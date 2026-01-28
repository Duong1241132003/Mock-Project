#include "service/player/SDLAudioPlayer.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

// Placeholder for future Decoders
#include "service/decoder/FFmpegAudioDecoder.h"
// #include "service/decoder/SdlMixerDecoder.h"

// For demonstration, we create a "NullDecoder" or fail if no decoder exists
class NullDecoder : public IAudioDecoder {
    // Empty implementation just to avoid crashes if file not found
    bool open(const std::string&) override { return false; }
    int readSamples(uint8_t*, int) override { return 0; }
    void seek(int) override {}
    void close() override {}
    int getSampleRate() const override { return 44100; }
    int getChannels() const override { return 2; }
    int getTotalDuration() const override { return 0; }
    int getCurrentPosition() const override { return 0; }
};

SDLAudioPlayer::SDLAudioPlayer() 
    : m_deviceId(0), m_isPlaying(false), m_isPaused(false), m_volume(SDL_MIX_MAXVOLUME) 
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "[SDL] Init failed: " << SDL_GetError() << "\n";
    }
}

SDLAudioPlayer::~SDLAudioPlayer() {
    stop();
    closeDevice();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

// =============================================================================
// FACTORY: Chọn Decoder (Kiến trúc Mở/Đóng - OCP)
// =============================================================================
std::unique_ptr<IAudioDecoder> SDLAudioPlayer::createDecoderForFile(const std::string& filePath) {
    std::string ext = std::filesystem::path(filePath).extension().string();
    
    // Logic chọn decoder dựa trên đuôi file
    // Sau này bạn sẽ include FFmpegAudioDecoder vào đây
    /*
    if (ext == ".mp3" || ext == ".flac") {
        return std::make_unique<FFmpegAudioDecoder>(); 
    }
    if (ext == ".wav") {
        return std::make_unique<LibSndFileDecoder>();
    }
    */
    
    return std::make_unique<FFmpegAudioDecoder>();
}

// =============================================================================
// CORE LOGIC
// =============================================================================

bool SDLAudioPlayer::load(const std::string& filePath) {
    stop(); // Reset state

    std::lock_guard<std::mutex> lock(m_mutex);

    // 1. Create specific decoder
    m_decoder = createDecoderForFile(filePath);

    // 2. Open file
    if (!m_decoder->open(filePath)) {
        std::cerr << "[Player] Failed to open file: " << filePath << "\n";
        return false;
    }

    // 3. Configure SDL Spec based on Decoder properties
    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = m_decoder->getSampleRate();
    desiredSpec.format = AUDIO_S16SYS; // Standard Signed 16-bit
    desiredSpec.channels = m_decoder->getChannels();
    desiredSpec.samples = 4096; // Buffer size (latency vs stability)
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = this;

    // 4. Open SDL Device
    // We assume the device supports the requested format. 
    // In real app, handle 'obtainedSpec' differences (resampling).
    m_deviceId = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &m_deviceSpec, 0);

    if (m_deviceId == 0) {
        std::cerr << "[SDL] Open device failed: " << SDL_GetError() << "\n";
        return false;
    }

    // Resize internal buffer to match callback request size
    // SDL passes 'len' bytes, which is typically samples * channels * 2 (16bit)
    // We reserve enough space to avoid reallocations in callback
    m_pcmBuffer.resize(m_deviceSpec.size);

    return true;
}

void SDLAudioPlayer::play() {
    if (m_deviceId == 0) return;
    
    m_isPlaying = true;
    m_isPaused = false;
    SDL_PauseAudioDevice(m_deviceId, 0); // 0 = Play
}

void SDLAudioPlayer::pause() {
    if (m_deviceId == 0) return;

    m_isPaused = true;
    SDL_PauseAudioDevice(m_deviceId, 1); // 1 = Pause
}

void SDLAudioPlayer::stop() {
    if (m_deviceId == 0) return;

    m_isPlaying = false;
    m_isPaused = false;
    SDL_PauseAudioDevice(m_deviceId, 1);
    
    closeDevice();
    
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_decoder) {
        m_decoder->close();
    }
}

void SDLAudioPlayer::closeDevice() {
    if (m_deviceId != 0) {
        SDL_CloseAudioDevice(m_deviceId);
        m_deviceId = 0;
    }
}

// =============================================================================
// AUDIO CALLBACK (Audio Thread)
// =============================================================================
void SDLAudioPlayer::audioCallback(void* userdata, Uint8* stream, int len) {
    auto* player = static_cast<SDLAudioPlayer*>(userdata);

    // 1. Initialize silence (tránh tiếng rác nếu không có dữ liệu)
    SDL_memset(stream, 0, len);

    // 2. Thread Safety Lock
    // We must lock because UI thread might be closing decoder or seeking
    std::lock_guard<std::mutex> lock(player->m_mutex);

    if (!player->m_decoder || player->m_isPaused || !player->m_isPlaying) {
        return;
    }

    // 3. Read PCM from Decoder
    // Ensure internal buffer is large enough
    if (player->m_pcmBuffer.size() < (size_t)len) {
        player->m_pcmBuffer.resize(len);
    }

    int bytesRead = player->m_decoder->readSamples(player->m_pcmBuffer.data(), len);

    if (bytesRead > 0) {
        // 4. Apply Volume & Mix to Stream
        // SDL_MixAudioFormat mixes from source (buffer) to destination (stream)
        // SDL_MIX_MAXVOLUME = 128
        SDL_MixAudioFormat(stream, 
                           player->m_pcmBuffer.data(), 
                           AUDIO_S16SYS, // Assumed format from load()
                           bytesRead, 
                           player->m_volume);
    } 
    
    // Nếu bytesRead < len, có nghĩa là hết bài (EOF) hoặc decoder chậm
    // Phần còn lại của stream đã được memset 0 ở trên (silence)
    if (bytesRead == 0) {
        // Handle EOF logic here (e.g., set a flag so UI knows to stop)
    }
}

// =============================================================================
// CONTROLS
// =============================================================================

void SDLAudioPlayer::setVolume(int volume) {
    int v = std::max(0, std::min(volume, 100));
    m_volume = (v * SDL_MIX_MAXVOLUME) / 100;
}

void SDLAudioPlayer::seek(int seconds) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_decoder) {
        m_decoder->seek(seconds);
    }
}

int SDLAudioPlayer::getCurrentTime() const {
    // Mutex might cause micro-stutter on UI if callback holds it too long
    // Ideally decoder->getCurrentPosition() should be atomic or fast
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_decoder) {
        return m_decoder->getCurrentPosition();
    }
    return 0;
}

int SDLAudioPlayer::getDuration() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_decoder) {
        return m_decoder->getTotalDuration();
    }
    return 0;
}

bool SDLAudioPlayer::isPlaying() const {
    return m_isPlaying && !m_isPaused;
}
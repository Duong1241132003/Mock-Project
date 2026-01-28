#ifndef SERVICE_PLAYER_SDL_AUDIO_PLAYER_H
#define SERVICE_PLAYER_SDL_AUDIO_PLAYER_H

#include "service/player/IAudioPlayer.h"
#include "service/decoder/IAudioDecoder.h"
#include <SDL2/SDL.h>
#include <mutex>
#include <memory>
#include <vector>
#include <atomic>

/**
 * @brief Concrete implementation of Audio Player using SDL2.
 * Acts as the consumer of PCM data provided by IAudioDecoder.
 */
class SDLAudioPlayer : public IAudioPlayer {
public:
    SDLAudioPlayer();
    ~SDLAudioPlayer() override;

    // --- IAudioPlayer Implementation ---
    bool load(const std::string& filePath) override;
    void play() override;
    void pause() override;
    void stop() override;

    void setVolume(int volume) override; // 0 - 100
    void seek(int seconds) override;

    int getCurrentTime() const override;
    int getDuration() const override;
    bool isPlaying() const override;

private:
    // --- Architecture Components ---
    std::unique_ptr<IAudioDecoder> m_decoder; // Polymorphic decoder
    
    // --- SDL Resources ---
    SDL_AudioDeviceID m_deviceId;
    SDL_AudioSpec m_deviceSpec;

    // --- State Management (Thread Safe) ---
    mutable std::mutex m_mutex; // Protects decoder access in callback
    std::atomic<bool> m_isPlaying;
    std::atomic<bool> m_isPaused;
    std::atomic<int> m_volume;  // SDL Volume (0-128)

    // --- Internal Buffer ---
    // Intermediate buffer to hold data read from decoder before mixing
    std::vector<uint8_t> m_pcmBuffer; 

    // --- Helpers ---
    void closeDevice();
    
    // Factory method to pick the right decoder (FFmpeg, LibSndFile, etc.)
    std::unique_ptr<IAudioDecoder> createDecoderForFile(const std::string& filePath);

    // --- SDL Callback ---
    static void audioCallback(void* userdata, Uint8* stream, int len);
};

#endif // SERVICE_PLAYER_SDL_AUDIO_PLAYER_H
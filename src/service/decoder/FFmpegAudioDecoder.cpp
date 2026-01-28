#include "service/decoder/FFmpegAudioDecoder.h"
#include <iostream>

FFmpegAudioDecoder::FFmpegAudioDecoder() {
    // Giảm bớt log rác của FFmpeg
    av_log_set_level(AV_LOG_ERROR);
}

FFmpegAudioDecoder::~FFmpegAudioDecoder() {
    close();
}

void FFmpegAudioDecoder::cleanUp() {
    if (m_swrCtx) { swr_free(&m_swrCtx); m_swrCtx = nullptr; }
    if (m_codecCtx) { avcodec_free_context(&m_codecCtx); m_codecCtx = nullptr; }
    if (m_formatCtx) { avformat_close_input(&m_formatCtx); m_formatCtx = nullptr; }
    if (m_frame) { av_frame_free(&m_frame); m_frame = nullptr; }
    if (m_packet) { av_packet_free(&m_packet); m_packet = nullptr; }
    
    m_sampleQueue.clear();
    m_audioStreamIndex = -1;
}

void FFmpegAudioDecoder::close() {
    cleanUp();
}

bool FFmpegAudioDecoder::open(const std::string& filePath) {
    close(); // Đảm bảo sạch sẽ trước khi mở file mới

    // 1. Open Input File
    if (avformat_open_input(&m_formatCtx, filePath.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "[FFmpeg] Failed to open file: " << filePath << "\n";
        return false;
    }

    // 2. Find Stream Info
    if (avformat_find_stream_info(m_formatCtx, nullptr) < 0) {
        std::cerr << "[FFmpeg] Failed to find stream info.\n";
        return false;
    }

    // 3. Find Audio Stream
    // [FIX ERROR] FFmpeg mới yêu cầu 'const AVCodec*'
    const AVCodec* codec = nullptr; 
    m_audioStreamIndex = av_find_best_stream(m_formatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);
    
    if (m_audioStreamIndex < 0) {
        std::cerr << "[FFmpeg] No audio stream found.\n";
        return false;
    }

    // 4. Create Codec Context
    m_codecCtx = avcodec_alloc_context3(codec);
    if (!m_codecCtx) {
        std::cerr << "[FFmpeg] Failed to allocate codec context.\n";
        return false;
    }

    avcodec_parameters_to_context(m_codecCtx, m_formatCtx->streams[m_audioStreamIndex]->codecpar);

    // 5. Open Codec
    if (avcodec_open2(m_codecCtx, codec, nullptr) < 0) {
        std::cerr << "[FFmpeg] Failed to open codec.\n";
        return false;
    }

    // 6. Alloc Resources
    m_frame = av_frame_alloc();
    m_packet = av_packet_alloc();

    // 7. Initialize Resampler (Chuyển đổi format audio sang chuẩn cho SDL)
    if (!initResampler()) {
        std::cerr << "[FFmpeg] Failed to init resampler.\n";
        return false;
    }

    return true;
}

bool FFmpegAudioDecoder::initResampler() {
    if (m_swrCtx) swr_free(&m_swrCtx);

    // [FIX WARNINGS] Sử dụng API mới của FFmpeg 5.0+ (AVChannelLayout & swr_alloc_set_opts2)
    
    // Output: Stereo (2 kênh), Signed 16-bit
    AVChannelLayout outChLayout = AV_CHANNEL_LAYOUT_STEREO;
    enum AVSampleFormat outSampleFmt = AV_SAMPLE_FMT_S16;
    int outSampleRate = m_codecCtx->sample_rate;

    // Input: Lấy từ Codec Context
    // Lưu ý: m_codecCtx->ch_layout là API mới thay thế channel_layout cũ
    AVChannelLayout inChLayout = m_codecCtx->ch_layout;
    
    // Fallback: Nếu input layout chưa được set (trường hợp hiếm), gán mặc định dựa trên số kênh
    if (inChLayout.nb_channels == 0) {
        av_channel_layout_default(&inChLayout, m_codecCtx->ch_layout.nb_channels);
    }

    // Tạo Resampler Context với API mới
    int ret = swr_alloc_set_opts2(
        &m_swrCtx,
        &outChLayout, outSampleFmt, outSampleRate,
        &inChLayout, m_codecCtx->sample_fmt, m_codecCtx->sample_rate,
        0, nullptr
    );

    if (ret < 0 || !m_swrCtx || swr_init(m_swrCtx) < 0) {
        std::cerr << "[FFmpeg] Failed to init resampler (swr_alloc_set_opts2).\n";
        return false;
    }
    return true;
}

int FFmpegAudioDecoder::readSamples(uint8_t* buffer, int bufferSize) {
    int bytesFilled = 0;

    while (bytesFilled < bufferSize) {
        // 1. Dùng dữ liệu còn dư trong hàng đợi trước
        if (!m_sampleQueue.empty()) {
            int toCopy = std::min((int)m_sampleQueue.size(), bufferSize - bytesFilled);
            
            for (int i = 0; i < toCopy; ++i) {
                buffer[bytesFilled + i] = m_sampleQueue.front();
                m_sampleQueue.pop_front();
            }
            bytesFilled += toCopy;
            
            if (bytesFilled == bufferSize) break;
        }

        // 2. Đọc frame tiếp theo từ file
        int ret = av_read_frame(m_formatCtx, m_packet);
        if (ret < 0) {
            // EOF (Hết bài) hoặc Lỗi
            break;
        }

        if (m_packet->stream_index == m_audioStreamIndex) {
            // Gửi packet nén vào decoder
            if (avcodec_send_packet(m_codecCtx, m_packet) == 0) {
                // Nhận frame giải nén (raw audio)
                while (avcodec_receive_frame(m_codecCtx, m_frame) == 0) {
                    
                    // 3. Resample frame sang S16 Stereo
                    // Tính toán số lượng mẫu đầu ra dự kiến
                    int maxDstSamples = av_rescale_rnd(
                        swr_get_delay(m_swrCtx, m_codecCtx->sample_rate) + m_frame->nb_samples,
                        m_codecCtx->sample_rate, m_codecCtx->sample_rate, AV_ROUND_UP
                    );

                    // Cấp phát buffer tạm thời cho dữ liệu đã resample
                    std::vector<uint8_t> resampledData(maxDstSamples * 2 * 2); // 2 channels * 2 bytes (16-bit)
                    uint8_t* outData[1] = { resampledData.data() };

                    // Thực hiện convert
                    int samplesConverted = swr_convert(
                        m_swrCtx,
                        outData, maxDstSamples,
                        (const uint8_t**)m_frame->data, m_frame->nb_samples
                    );

                    if (samplesConverted > 0) {
                        int bytesConverted = samplesConverted * 2 * 2; // samples * channels * bytes_per_sample
                        pushToQueue(resampledData.data(), bytesConverted);
                    }
                }
            }
        }
        av_packet_unref(m_packet);
    }
    
    return bytesFilled;
}

void FFmpegAudioDecoder::pushToQueue(uint8_t* data, int size) {
    for (int i = 0; i < size; ++i) {
        m_sampleQueue.push_back(data[i]);
    }
}

void FFmpegAudioDecoder::seek(int seconds) {
    if (!m_formatCtx) return;

    // Tính toán timestamp đích
    int64_t targetTs = av_rescale_q(seconds * AV_TIME_BASE, AV_TIME_BASE_Q, m_formatCtx->streams[m_audioStreamIndex]->time_base);
    
    // Seek
    if (av_seek_frame(m_formatCtx, m_audioStreamIndex, targetTs, AVSEEK_FLAG_BACKWARD) >= 0) {
        // Xả bộ đệm của codec để tránh bị nhiễu âm thanh cũ
        avcodec_flush_buffers(m_codecCtx);
        m_sampleQueue.clear();
    }
}

int FFmpegAudioDecoder::getSampleRate() const {
    return m_codecCtx ? m_codecCtx->sample_rate : 44100;
}

int FFmpegAudioDecoder::getChannels() const {
    // Chúng ta luôn resample về Stereo (2 kênh)
    return 2; 
}

int FFmpegAudioDecoder::getTotalDuration() const {
    if (!m_formatCtx) return 0;
    return m_formatCtx->duration / AV_TIME_BASE;
}

int FFmpegAudioDecoder::getCurrentPosition() const {
    // Đây là ước lượng. Để chính xác hơn cần theo dõi số byte đã đọc.
    if (!m_formatCtx) return 0;
    return 0; // TODO: Implement chính xác nếu cần thanh seekbar mượt
}
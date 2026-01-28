#ifndef SERVICE_DECODER_FFMPEG_AUDIO_DECODER_H
#define SERVICE_DECODER_FFMPEG_AUDIO_DECODER_H

#include "service/decoder/IAudioDecoder.h"
#include <vector>
#include <deque>

// FFmpeg is written in C, so we need extern "C" for C++ linking
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}

class FFmpegAudioDecoder : public IAudioDecoder {
public:
    FFmpegAudioDecoder();
    ~FFmpegAudioDecoder() override;

    bool open(const std::string& filePath) override;
    int readSamples(uint8_t* buffer, int bufferSize) override;
    void seek(int seconds) override;
    void close() override;

    int getSampleRate() const override;
    int getChannels() const override;
    int getTotalDuration() const override;
    int getCurrentPosition() const override;

private:
    // --- FFmpeg Contexts ---
    AVFormatContext* m_formatCtx = nullptr;
    AVCodecContext* m_codecCtx = nullptr;
    SwrContext* m_swrCtx = nullptr;

    int m_audioStreamIndex = -1;
    
    // --- Data Holders ---
    AVFrame* m_frame = nullptr;       // Decoded raw audio (in source format)
    AVPacket* m_packet = nullptr;     // Compressed data (mp3/aac chunk)

    // --- Internal Buffer ---
    // FFmpeg decodes a full frame (e.g., 1152 samples), but SDL asks for 4096 bytes.
    // We need a queue to store the leftover bytes from the last frame.
    std::deque<uint8_t> m_sampleQueue;

    // --- Helpers ---
    void cleanUp();
    bool initResampler();
    void pushToQueue(uint8_t* data, int size);
};

#endif // SERVICE_DECODER_FFMPEG_AUDIO_DECODER_H
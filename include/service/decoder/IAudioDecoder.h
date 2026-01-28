#ifndef SERVICE_DECODER_IAUDIODECODER_H
#define SERVICE_DECODER_IAUDIODECODER_H

#include <string>
#include <cstdint>

/**
 * @brief Interface for audio decoding logic.
 * Abstracts away specific libraries (FFmpeg, libsndfile, mpg123).
 */
class IAudioDecoder {
public:
    virtual ~IAudioDecoder() = default;

    /**
     * @brief Opens an audio file and prepares for decoding.
     * @return true if file is supported and opened successfully.
     */
    virtual bool open(const std::string& filePath) = 0;

    /**
     * @brief Reads PCM samples into the provided buffer.
     * @param buffer Pointer to the destination buffer.
     * @param bufferSize Size of the buffer in bytes.
     * @return Number of bytes actually read. 0 means EOF or Error.
     */
    virtual int readSamples(uint8_t* buffer, int bufferSize) = 0;

    /**
     * @brief Seeks to a specific timestamp.
     * @param seconds Target time in seconds.
     */
    virtual void seek(int seconds) = 0;

    /**
     * @brief Close the file and release resources.
     */
    virtual void close() = 0;

    // --- Audio Properties (Needed to open SDL Device) ---
    virtual int getSampleRate() const = 0; // e.g., 44100
    virtual int getChannels() const = 0;   // e.g., 2 (Stereo)
    
    // --- Metadata for Playback ---
    virtual int getTotalDuration() const = 0; // seconds
    virtual int getCurrentPosition() const = 0; // seconds
};

#endif // SERVICE_DECODER_IAUDIODECODER_H
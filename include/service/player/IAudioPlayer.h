#ifndef SERVICE_PLAYER_IAUDIOPLAYER_H
#define SERVICE_PLAYER_IAUDIOPLAYER_H

#include <string>

/**
 * @brief Interface abstraction for the Audio Backend.
 * Decouples the logic from specific libraries (SDL2, SFML, etc.).
 */
class IAudioPlayer {
public:
    virtual ~IAudioPlayer() = default;

    // --- Core Operations ---
    virtual bool load(const std::string& filePath) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

    // --- Configuration ---
    virtual void setVolume(int volume) = 0; // 0 - 100
    virtual void seek(int seconds) = 0;

    // --- State Query ---
    virtual int getCurrentTime() const = 0; // in seconds
    virtual int getDuration() const = 0;    // in seconds
    virtual bool isPlaying() const = 0;
};

#endif // SERVICE_PLAYER_IAUDIOPLAYER_H
#ifndef CONTROLLER_PLAYBACK_CONTROLLER_H
#define CONTROLLER_PLAYBACK_CONTROLLER_H

#include <vector>
#include <memory>
#include <mutex>
#include <random>
#include <algorithm>
#include "model/MediaFile.h"
#include "service/player/IAudioPlayer.h"
#include "service/player/AudioThreadWorker.h"

class PlaybackController {
public:
    explicit PlaybackController(IAudioPlayer& player);
    ~PlaybackController();

    // --- Playlist Management ---
    void setPlaylist(const std::vector<MediaFile>& playlist);
    void playIndex(size_t index);

    // --- Controls ---
    void play();
    void pause(); // Toggle logic implemented here
    void stop();
    void next();
    void previous();

    void setLoop(bool enabled);
    void setShuffle(bool enabled);

    void setVolume(int volume);
    void seek(int seconds);

    // --- State Queries ---
    int getCurrentTime() const;
    int getDuration() const;
    bool isPlaying() const;
    
    // Lấy bài hát hiện tại để UI hiển thị
    const MediaFile* getCurrentMedia() const;

private:
    // Dependencies
    IAudioPlayer& m_player; // Direct access mainly for queries
    std::unique_ptr<AudioThreadWorker> m_worker;

    // State Data (Protected by mutex since Worker might callback here)
    mutable std::mutex m_controllerMutex;
    std::vector<MediaFile> m_playlist;
    size_t m_currentIndex = 0;
    
    bool m_loopEnabled = false;
    bool m_shuffleEnabled = false;

    // Random generator for shuffle
    std::mt19937 m_rng;

    // Helpers
    void playInternal(); // Helper to send play command to worker
    size_t calculateNextIndex();
    size_t calculatePrevIndex();
};

#endif // CONTROLLER_PLAYBACK_CONTROLLER_H
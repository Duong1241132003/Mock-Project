#ifndef CONTROLLER_PLAYBACK_CONTROLLER_H
#define CONTROLLER_PLAYBACK_CONTROLLER_H

#include <mutex>
#include <functional>
#include <string>
#include <vector>
#include <random>
#include <memory>
#include <algorithm>

// Dependency Interfaces
#include "service/player/AudioThreadWorker.h"
#include "service/metadata/IMetadataService.h"
#include "model/MediaFile.h"

enum class PlaybackState {
    Stopped,
    Playing,
    Paused
};

class PlaybackController {
public:
    // [THAY ĐỔI] Constructor chỉ còn 2 tham số (Audio & Metadata)
    PlaybackController(AudioThreadWorker& audioWorker,
                       IMetadataService& metadataService);

    ~PlaybackController();

    // --- Playlist Management ---
    void setPlaylist(const std::vector<MediaFile>& playlist);

    // --- Controls ---
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void seek(int seconds);
    void setVolume(int volume);

    // --- Modes ---
    void setLoopEnabled(bool enabled);
    void setShuffleEnabled(bool enabled);

    // --- Callbacks (Output to UI) ---
    void setOnSongChanged(std::function<void(const std::string& title, const std::string& artist)> cb);
    void setOnPlaybackStateChanged(std::function<void(bool isPlaying)> cb);

    // --- Getters ---
    PlaybackState getState() const;

private:
    // Dependencies
    AudioThreadWorker& m_audioWorker;
    IMetadataService& m_metadataService;
    // Đã xóa S32K144ProtocolHandler

    // State
    mutable std::mutex m_mutex;
    std::vector<MediaFile> m_playlist;
    size_t m_currentIndex = 0;
    PlaybackState m_state = PlaybackState::Stopped;

    bool m_isLooping = false;
    bool m_isShuffling = false;
    int m_currentVolume = 50;

    std::mt19937 m_rng;

    // UI Callbacks
    std::function<void(const std::string&, const std::string&)> m_uiSongChangedCb;
    std::function<void(bool)> m_uiStateChangedCb;

    // Helpers
    void playIndex(size_t index);
    size_t calculateNextIndex();
    size_t calculatePreviousIndex();
    void updateMetadataAndNotify(MediaFile& file);
    void stopInternal(); // Helper for deadlock-free internal stop
};

#endif // CONTROLLER_PLAYBACK_CONTROLLER_H
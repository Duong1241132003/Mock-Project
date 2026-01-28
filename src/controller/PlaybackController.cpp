#include "controller/PlaybackController.h"
#include <iostream>

PlaybackController::PlaybackController(AudioThreadWorker& audioWorker,
                                       IMetadataService& metadataService)
    : m_audioWorker(audioWorker),
      m_metadataService(metadataService)
{
    std::random_device rd;
    m_rng = std::mt19937(rd());

    // Auto-Next callback từ Audio Thread
    m_audioWorker.setOnTrackFinishedCallback([this]() {
        this->next(); 
    });
}

PlaybackController::~PlaybackController() {
    stop();
}

void PlaybackController::setPlaylist(const std::vector<MediaFile>& playlist) {
    std::lock_guard<std::mutex> lock(m_mutex);
    stopInternal();
    m_playlist = playlist;
    m_currentIndex = 0;
}

void PlaybackController::play() {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_playlist.empty()) return;

    if (m_state == PlaybackState::Paused) {
        m_audioWorker.requestPause(); // Resume
        m_state = PlaybackState::Playing;
        if (m_uiStateChangedCb) m_uiStateChangedCb(true);
    } 
    else if (m_state == PlaybackState::Stopped) {
        playIndex(m_currentIndex);
    }
}

void PlaybackController::pause() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_state == PlaybackState::Playing) {
        m_audioWorker.requestPause();
        m_state = PlaybackState::Paused;
        if (m_uiStateChangedCb) m_uiStateChangedCb(false);
    }
    else if (m_state == PlaybackState::Paused) {
        // Nếu đang pause thì bấm pause lần nữa sẽ resume (Toggle)
        // Cần nhả lock trước khi gọi play() để tránh deadlock recursive
        // Nhưng ở đây play() dùng lock_guard cùng mutex, nếu gọi trực tiếp sẽ deadlock
        // GIẢI PHÁP: Copy logic resume vào đây hoặc unlock trước
        m_audioWorker.requestPause(); // Resume logic bên worker
        m_state = PlaybackState::Playing;
        if (m_uiStateChangedCb) m_uiStateChangedCb(true);
    }
}

void PlaybackController::stop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    stopInternal();
}

void PlaybackController::stopInternal() {
    m_audioWorker.requestStop();
    m_state = PlaybackState::Stopped;
    if (m_uiStateChangedCb) m_uiStateChangedCb(false);
}

void PlaybackController::next() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_playlist.empty()) return;

    m_currentIndex = calculateNextIndex();
    playIndex(m_currentIndex);
}

void PlaybackController::previous() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_playlist.empty()) return;

    m_currentIndex = calculatePreviousIndex();
    playIndex(m_currentIndex);
}

void PlaybackController::seek(int seconds) {
    m_audioWorker.requestSeek(seconds);
}

void PlaybackController::setVolume(int volume) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentVolume = std::max(0, std::min(volume, 100));
    m_audioWorker.requestSetVolume(m_currentVolume);
}

// --- INTERNAL HELPERS ---

void PlaybackController::playIndex(size_t index) {
    if (index >= m_playlist.size()) return;

    MediaFile& file = m_playlist[index];
    std::string path = file.getPath();

    m_audioWorker.requestPlay(path);
    m_state = PlaybackState::Playing;

    updateMetadataAndNotify(file);
}

void PlaybackController::updateMetadataAndNotify(MediaFile& file) {
    // Đọc metadata
    if (!m_metadataService.readMetadata(file)) {
        // Fallback logic nếu cần
    }

    std::string title = file.getTitle();
    std::string artist = file.getArtist();

    // Chỉ gửi lên UI, không gửi Hardware nữa
    if (m_uiSongChangedCb) {
        m_uiSongChangedCb(title, artist);
    }
    if (m_uiStateChangedCb) {
        m_uiStateChangedCb(true);
    }
}

size_t PlaybackController::calculateNextIndex() {
    size_t count = m_playlist.size();
    if (count == 0) return 0;

    if (m_isShuffling) {
        std::uniform_int_distribution<size_t> dist(0, count - 1);
        return dist(m_rng);
    }

    if (m_currentIndex + 1 >= count) {
        return m_isLooping ? 0 : m_currentIndex;
    }
    return m_currentIndex + 1;
}

size_t PlaybackController::calculatePreviousIndex() {
    size_t count = m_playlist.size();
    if (count == 0) return 0;

    if (m_isShuffling) {
        std::uniform_int_distribution<size_t> dist(0, count - 1);
        return dist(m_rng);
    }

    if (m_currentIndex == 0) {
        return m_isLooping ? count - 1 : 0;
    }
    return m_currentIndex - 1;
}

// --- SETTERS ---

void PlaybackController::setLoopEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isLooping = enabled;
}

void PlaybackController::setShuffleEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isShuffling = enabled;
}

void PlaybackController::setOnSongChanged(std::function<void(const std::string&, const std::string&)> cb) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_uiSongChangedCb = cb;
}

void PlaybackController::setOnPlaybackStateChanged(std::function<void(bool)> cb) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_uiStateChangedCb = cb;
}

PlaybackState PlaybackController::getState() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_state;
}
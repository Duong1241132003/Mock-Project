#include "controller/PlaybackController.h"
#include <iostream>

PlaybackController::PlaybackController(IAudioPlayer& player)
    : m_player(player) 
{
    // 1. Khởi tạo Random Device
    std::random_device rd;
    m_rng = std::mt19937(rd());

    // 2. Khởi tạo Worker
    // [FIX LỖI]: Truyền m_player vào constructor của Worker
    m_worker = std::make_unique<AudioThreadWorker>(m_player);

    // 3. Đăng ký Auto-Next
    // Khi Worker phát hiện hết bài, nó sẽ gọi callback này
    m_worker->setOnTrackFinishedCallback([this]() {
        std::cout << "[Controller] Track finished. Auto-Next.\n";
        this->next();
    });
}

PlaybackController::~PlaybackController() {
    // Worker tự hủy trong destructor của unique_ptr
}

void PlaybackController::setPlaylist(const std::vector<MediaFile>& playlist) {
    std::lock_guard<std::mutex> lock(m_controllerMutex);
    stop(); // Dừng bài cũ
    m_playlist = playlist;
    m_currentIndex = 0;
}

void PlaybackController::playIndex(size_t index) {
    std::lock_guard<std::mutex> lock(m_controllerMutex);
    if (index >= m_playlist.size()) return;
    
    m_currentIndex = index;
    playInternal();
}

void PlaybackController::play() {
    std::lock_guard<std::mutex> lock(m_controllerMutex);
    if (m_playlist.empty()) return;

    if (m_player.isPlaying()) {
        // Đã play rồi thì không làm gì
    } else {
        // [FIX LỖI]: Dùng requestPause (đóng vai trò resume) thay vì enqueue
        m_worker->requestPause(); 
    }
}

void PlaybackController::pause() {
    // [FIX LỖI]: Dùng requestPause thay vì enqueue
    m_worker->requestPause();
}

void PlaybackController::stop() {
    // [FIX LỖI]: Dùng requestStop thay vì enqueue
    m_worker->requestStop();
}

void PlaybackController::next() {
    std::lock_guard<std::mutex> lock(m_controllerMutex);
    if (m_playlist.empty()) return;

    m_currentIndex = calculateNextIndex();
    playInternal();
}

void PlaybackController::previous() {
    std::lock_guard<std::mutex> lock(m_controllerMutex);
    if (m_playlist.empty()) return;

    m_currentIndex = calculatePrevIndex();
    playInternal();
}

// --- Logic tính toán Index ---

size_t PlaybackController::calculateNextIndex() {
    if (m_shuffleEnabled) {
        // Random trong khoảng playlist
        std::uniform_int_distribution<size_t> dist(0, m_playlist.size() - 1);
        return dist(m_rng);
    }
    
    // Normal + Loop
    if (m_currentIndex + 1 >= m_playlist.size()) {
        return m_loopEnabled ? 0 : m_currentIndex; // Loop về đầu hoặc đứng yên
    }
    return m_currentIndex + 1;
}

size_t PlaybackController::calculatePrevIndex() {
    if (m_currentIndex == 0) {
        return m_loopEnabled ? m_playlist.size() - 1 : 0;
    }
    return m_currentIndex - 1;
}

void PlaybackController::playInternal() {
    // Hàm này giả định mutex đã được lock bên ngoài
    if (m_playlist.empty()) return;
    
    std::string path = m_playlist[m_currentIndex].getPath();
    
    // [FIX LỖI]: Dùng requestPlay thay vì enqueue
    m_worker->requestPlay(path);
}

// --- Setters ---

void PlaybackController::setLoop(bool enabled) {
    std::lock_guard<std::mutex> lock(m_controllerMutex);
    m_loopEnabled = enabled;
}

void PlaybackController::setShuffle(bool enabled) {
    std::lock_guard<std::mutex> lock(m_controllerMutex);
    m_shuffleEnabled = enabled;
}

void PlaybackController::setVolume(int volume) {
    // [FIX LỖI]: Dùng requestSetVolume thay vì enqueue
    m_worker->requestSetVolume(volume);
}

void PlaybackController::seek(int seconds) {
    // [FIX LỖI]: Dùng requestSeek thay vì enqueue
    m_worker->requestSeek(seconds);
}

// --- Getters ---

int PlaybackController::getCurrentTime() const {
    return m_player.getCurrentTime();
}

int PlaybackController::getDuration() const {
    return m_player.getDuration();
}

bool PlaybackController::isPlaying() const {
    return m_player.isPlaying();
}

const MediaFile* PlaybackController::getCurrentMedia() const {
    std::lock_guard<std::mutex> lock(m_controllerMutex);
    if (m_currentIndex < m_playlist.size()) {
        return &m_playlist[m_currentIndex];
    }
    return nullptr;
}
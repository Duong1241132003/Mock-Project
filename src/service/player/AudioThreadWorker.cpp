#include "service/player/AudioThreadWorker.h"
#include <iostream>
#include <chrono>

AudioThreadWorker::AudioThreadWorker(IAudioPlayer& player)
    : m_player(player), m_running(false) 
{
    start();
}

AudioThreadWorker::~AudioThreadWorker() {
    stop();
}

void AudioThreadWorker::start() {
    if (m_running) return;
    m_running = true;
    m_workerThread = std::thread(&AudioThreadWorker::threadLoop, this);
}

void AudioThreadWorker::stop() {
    if (!m_running) return;
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_running = false;
        m_currentCommand = WorkerCommand::None;
    }
    m_cv.notify_one();

    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

// --- Request Implementations ---

void AudioThreadWorker::requestPlay(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentCommand = WorkerCommand::PlayNewFile;
    m_pendingPath = filePath;
    m_cv.notify_one();
}

void AudioThreadWorker::requestPause() {
    std::lock_guard<std::mutex> lock(m_mutex);
    // Logic đơn giản hóa: Worker nhận lệnh, việc check state để controller lo hoặc xử lý sau
    if (m_player.isPlaying()) {
        m_currentCommand = WorkerCommand::Pause;
    } else {
        m_currentCommand = WorkerCommand::Resume;
    }
    m_cv.notify_one();
}

void AudioThreadWorker::requestStop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentCommand = WorkerCommand::Stop;
    m_cv.notify_one();
}

void AudioThreadWorker::requestSeek(int seconds) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentCommand = WorkerCommand::Seek;
    m_pendingVal = seconds;
    m_cv.notify_one();
}

void AudioThreadWorker::requestSetVolume(int volume) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentCommand = WorkerCommand::SetVolume;
    m_pendingVal = volume;
    m_cv.notify_one();
}

void AudioThreadWorker::setOnTrackFinishedCallback(std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_onTrackFinished = callback;
}

// --- The Core Loop ---

void AudioThreadWorker::threadLoop() {
    while (m_running) {
        std::unique_lock<std::mutex> lock(m_mutex);

        // Timeout 500ms để check trạng thái bài hát liên tục
        m_cv.wait_for(lock, std::chrono::milliseconds(500), [this] {
            return !m_running || m_currentCommand != WorkerCommand::None;
        });

        if (!m_running) break;

        // 1. Lấy lệnh
        WorkerCommand cmd = m_currentCommand;
        std::string path = m_pendingPath;
        int val = m_pendingVal;
        
        m_currentCommand = WorkerCommand::None; 
        
        lock.unlock(); // Mở khóa để thực thi lệnh nặng

        // 2. Thực thi lệnh
        switch (cmd) {
            case WorkerCommand::PlayNewFile:
                if (m_player.load(path)) {
                    m_player.play();
                }
                break;
            case WorkerCommand::Pause:
                m_player.pause();
                break;
            case WorkerCommand::Resume:
                m_player.play();
                break;
            case WorkerCommand::Stop:
                m_player.stop();
                break;
            case WorkerCommand::Seek:
                m_player.seek(val);
                break;
            case WorkerCommand::SetVolume:
                m_player.setVolume(val);
                break;
            case WorkerCommand::None:
            default:
                // Auto-Next Check
                if (m_player.getDuration() > 0 && 
                    m_player.getCurrentTime() >= m_player.getDuration() - 1) { 
                    
                    if (m_onTrackFinished) {
                        m_onTrackFinished();
                    }
                }
                break;
        }
    }
}
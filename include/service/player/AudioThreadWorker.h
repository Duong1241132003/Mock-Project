#ifndef SERVICE_PLAYER_AUDIO_THREAD_WORKER_H
#define SERVICE_PLAYER_AUDIO_THREAD_WORKER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <atomic>
#include <functional>
#include "service/player/IAudioPlayer.h"

// Các loại lệnh mà Worker có thể nhận
enum class WorkerCommand {
    None,
    PlayNewFile, // Load và Play file mới
    Pause,       // Pause/Resume toggle
    Resume,
    Stop,
    Seek,
    SetVolume
};

/**
 * @brief Handles audio operations on a dedicated background thread.
 * Prevents the UI thread from blocking during heavy I/O or decoding operations.
 */
class AudioThreadWorker {
public:
    explicit AudioThreadWorker(IAudioPlayer& player);
    ~AudioThreadWorker();

    void start();
    void stop();

    // --- Requests (Called from UI Thread) ---
    void requestPlay(const std::string& filePath);
    void requestPause(); 
    void requestStop();
    void requestSeek(int seconds);
    void requestSetVolume(int volume);

    // --- Callback ---
    // Đăng ký hàm sẽ được gọi khi bài hát kết thúc tự nhiên
    void setOnTrackFinishedCallback(std::function<void()> callback);

private:
    void threadLoop();

    IAudioPlayer& m_player;
    
    // Threading primitives
    std::thread m_workerThread;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_running;

    // Pending Command State
    WorkerCommand m_currentCommand = WorkerCommand::None;
    std::string m_pendingPath;
    int m_pendingVal = 0; // Dùng chung cho Volume hoặc Seek time

    // Callback
    std::function<void()> m_onTrackFinished;
};

#endif // SERVICE_PLAYER_AUDIO_THREAD_WORKER_H
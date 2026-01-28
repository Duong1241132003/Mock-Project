#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>
#include <mutex>

// Modules
#include "service/scanner/LinuxFileSystem.h"
#include "service/scanner/FileScanner.h"
#include "service/metadata/TagLibMetadataService.h"
#include "service/player/SDLAudioPlayer.h"
#include "service/player/AudioThreadWorker.h"
#include "controller/PlaybackController.h"

// UI State Helper
struct ConsoleUIState {
    std::string currentTitle = "None";
    std::string currentArtist = "Unknown";
    bool isPlaying = false;
    int currentVolume = 50;
    std::mutex uiMutex;
} uiState;

void clearScreen() {
    std::cout << "\033[2J\033[1;1H"; 
}

void printStatus() {
    clearScreen();
    std::lock_guard<std::mutex> lock(uiState.uiMutex);

    std::cout << "========================================\n";
    std::cout << "   SIMPLE MEDIA PLAYER (SOFTWARE ONLY)  \n";
    std::cout << "========================================\n";

    if (uiState.currentTitle != "None") {
        std::cout << "NP: " << uiState.currentTitle << " - " << uiState.currentArtist << "\n";
        std::cout << "State: " << (uiState.isPlaying ? "[PLAYING]" : "[PAUSED]") << "\n";
        std::cout << "Vol: " << uiState.currentVolume << "%\n";
    } else {
        std::cout << "NP: [Stopped / No File]\n";
    }

    std::cout << "----------------------------------------\n";
    std::cout << "Controls:\n";
    std::cout << "  [n] Next       [p] Previous\n";
    std::cout << "  [space/t] Pause  [s] Stop\n";
    std::cout << "  [+] Vol Up     [-] Vol Down\n";
    std::cout << "  [q] Quit\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Enter command: ";
}

int main(int argc, char* argv[]) {
    // 1. SETUP DATA SOURCE
    // Nhớ copy nhạc vào đây trước khi chạy
    std::string scanPath = "/home/duong/Music"; 
    if (argc > 1) scanPath = argv[1];

    std::cout << "[Init] Scanning: " << scanPath << "...\n";
    auto fs = std::make_shared<LinuxFileSystem>();
    FileScanner scanner(fs);
    std::vector<MediaFile> playlist = scanner.scanDirectory(scanPath);

    if (playlist.empty()) {
        std::cerr << "[Error] No music files found in " << scanPath << "\n";
        std::cerr << "Tip: Copy mp3 files to /home/duong/Music and try again.\n";
        return 1;
    }


    // 2. SETUP MODULES
    std::cout << "[Init] Initializing TagLibMetadataService...\n";
    TagLibMetadataService metadataService;

    std::cout << "[Init] Initializing SDLAudioPlayer...\n";
    SDLAudioPlayer sdlPlayer;
    std::cout << "[Init] Initializing AudioThreadWorker...\n";
    AudioThreadWorker audioWorker(sdlPlayer);

    // 3. INJECT DEPENDENCIES (Chỉ còn 2 tham số)
    std::cout << "[Init] Initializing PlaybackController...\n";
    PlaybackController controller(audioWorker, metadataService);

    // 4. SETUP UI CALLBACKS
    controller.setOnSongChanged([](const std::string& title, const std::string& artist) {
        std::lock_guard<std::mutex> lock(uiState.uiMutex);
        uiState.currentTitle = title;
        uiState.currentArtist = artist;
    });

    controller.setOnPlaybackStateChanged([](bool isPlaying) {
        std::lock_guard<std::mutex> lock(uiState.uiMutex);
        uiState.isPlaying = isPlaying;
    });

    // 5. START SYSTEM
    std::cout << "[Init] Setting playlist...\n";
    controller.setPlaylist(playlist);
    
    std::cout << "[Init] Starting playback...\n";
    controller.play();

    // 6. MAIN LOOP
    char cmd;
    bool running = true;

    while (running) {
        printStatus();
        std::cin >> cmd;

        switch (cmd) {
            case 'n': controller.next(); break;
            case 'p': controller.previous(); break;
            case 's': controller.stop(); break;
            case ' ': 
            case 't': controller.pause(); break; 
            case '+': 
                {
                    std::lock_guard<std::mutex> lock(uiState.uiMutex);
                    uiState.currentVolume = std::min(100, uiState.currentVolume + 10);
                    controller.setVolume(uiState.currentVolume);
                }
                break;
            case '-': 
                {
                    std::lock_guard<std::mutex> lock(uiState.uiMutex);
                    uiState.currentVolume = std::max(0, uiState.currentVolume - 10);
                    controller.setVolume(uiState.currentVolume);
                }
                break;
            case 'q': running = false; break;
            default: break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    controller.stop();
    return 0;
}
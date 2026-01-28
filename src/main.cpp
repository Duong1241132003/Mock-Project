// // // // /* Test Module 1*/
// // // // #include <iostream>
// // // // #include <vector>
// // // // #include <memory> // For std::shared_ptr

// // // // // Include các header theo đúng đường dẫn thư mục
// // // // #include "service/scanner/IFileSystem.h"
// // // // #include "service/scanner/LinuxFileSystem.h"
// // // // #include "service/scanner/FileScanner.h"
// // // // #include "model/MediaFile.h"

// // // // // Helper function to convert Enum to String for display
// // // // std::string mediaTypeToString(MediaType type) {
// // // //     switch (type) {
// // // //         case MediaType::Audio: return "[Audio]";
// // // //         case MediaType::Video: return "[Video]";
// // // //         default: return "[Unknown]";
// // // //     }
// // // // }

// // // // int main(int argc, char* argv[]) {
// // // //     // 1. Setup Data Source (Dependency)
// // // //     // Create the concrete implementation of FileSystem
// // // //     auto linuxFS = std::make_shared<LinuxFileSystem>();

// // // //     // 2. Inject Dependency
// // // //     // Inject the FileSystem into the Scanner
// // // //     // FileScanner doesn't know it's using LinuxFileSystem, it just sees IFileSystem
// // // //     FileScanner scanner(linuxFS);

// // // //     // 3. Define path to scan
// // // //     // Default to current directory if no argument provided
// // // //     std::string pathToString = "."; 
// // // //     if (argc > 1) {
// // // //         pathToString = argv[1];
// // // //     }

// // // //     std::cout << "Starting scan at: " << pathToString << "\n";
// // // //     std::cout << "----------------------------------------\n";

// // // //     // 4. Execute Logic
// // // //     std::vector<MediaFile> mediaFiles = scanner.scanDirectory(pathToString);

// // // //     // 5. Display Results
// // // //     if (mediaFiles.empty()) {
// // // //         std::cout << "No media files found.\n";
// // // //     } else {
// // // //         int count = 0;
// // // //         for (const auto& file : mediaFiles) {
// // // //             count++;
// // // //             std::cout << count << ". " 
// // // //                       << mediaTypeToString(file.getType()) << " " 
// // // //                       << file.getFileName() << "\n"
// // // //                       << "   Path: " << file.getPath() << "\n";
// // // //         }
// // // //         std::cout << "----------------------------------------\n";
// // // //         std::cout << "Total found: " << count << " files.\n";
// // // //     }

// // // //     return 0;
// // // // }


// // // /* Test Module 2 */
// // // #include <iostream>
// // // #include <vector>
// // // #include <memory>
// // // #include <iomanip> // std::setw

// // // // Module 1 Headers
// // // #include "service/scanner/IFileSystem.h"
// // // #include "service/scanner/LinuxFileSystem.h"
// // // #include "service/scanner/FileScanner.h"

// // // // Module 2 Headers
// // // #include "model/MediaLibrary.h"
// // // #include "model/PlaybackHistory.h"

// // // // --- Helper: Hàm in danh sách ra màn hình cho đẹp ---
// // // void printList(const std::vector<MediaFile>& list, const std::string& title) {
// // //     std::cout << "\n=== " << title << " (" << list.size() << " items) ===\n";
// // //     if (list.empty()) {
// // //         std::cout << "  (Empty)\n";
// // //         return;
// // //     }
    
// // //     // In Header
// // //     std::cout << std::left 
// // //               << std::setw(30) << "TITLE" 
// // //               << std::setw(20) << "ARTIST" 
// // //               << std::setw(20) << "ALBUM" << "\n";
// // //     std::cout << std::string(70, '-') << "\n";

// // //     // In Data
// // //     for (const auto& file : list) {
// // //         // Cắt chuỗi nếu quá dài để hiển thị đẹp
// // //         std::string t = file.getTitle().substr(0, 29);
// // //         std::string a = file.getArtist().substr(0, 19);
// // //         std::string al = file.getAlbum().substr(0, 19);
        
// // //         std::cout << std::left 
// // //                   << std::setw(30) << t 
// // //                   << std::setw(20) << a 
// // //                   << std::setw(20) << al << "\n";
// // //     }
// // // }

// // // // --- Helper: Hàm giả lập Metadata (Vì chưa có module đọc tag) ---
// // // void mockMetadata(std::vector<MediaFile>& files) {
// // //     // Gán random Artist/Album để test tính năng Sort/Search
// // //     std::vector<std::string> artists = {"Son Tung MTP", "Adele", "Taylor Swift", "Den Vau", "Imagine Dragons"};
// // //     std::vector<std::string> albums = {"Sky Tour", "25", "1989", "Dong Vui", "Evolve"};

// // //     for (size_t i = 0; i < files.size(); ++i) {
// // //         // Gán Artist theo vòng lặp
// // //         files[i].setArtist(artists[i % artists.size()]);
// // //         // Gán Album theo vòng lặp ngược
// // //         files[i].setAlbum(albums[(files.size() - i) % albums.size()]);
        
// // //         // Title giữ nguyên là filename
// // //     }
// // //     std::cout << ">> [DEBUG] Generated fake metadata for testing.\n";
// // // }

// // // int main() {
// // //     // ==========================================
// // //     // PHẦN 1: SCAN DATA (Module 1)
// // //     // ==========================================
// // //     std::string scanPath = "/media/sf_Playlist/Music/000";
// // //     std::cout << "1. Scanning directory: " << scanPath << " ...\n";

// // //     auto fs = std::make_shared<LinuxFileSystem>();
// // //     FileScanner scanner(fs);
    
// // //     // Quét file thật
// // //     std::vector<MediaFile> rawFiles = scanner.scanDirectory(scanPath);
    
// // //     if (rawFiles.empty()) {
// // //         std::cout << "[ERROR] No files found in " << scanPath << "\n";
// // //         std::cout << "Please check if the path exists and contains media files.\n";
// // //         return 0; // Dừng nếu không có dữ liệu
// // //     }

// // //     std::cout << ">> Found " << rawFiles.size() << " files.\n";

// // //     // ==========================================
// // //     // PHẦN 2: SETUP LIBRARY (Module 2)
// // //     // ==========================================
    
// // //     // Giả lập metadata để test sort/search
// // //     mockMetadata(rawFiles);

// // //     MediaLibrary library;
// // //     library.setMediaList(rawFiles);

// // //     // --- Test 2.1: Pagination ---
// // //     std::cout << "\n--- TEST PAGINATION ---\n";
// // //     // Lấy trang 1, mỗi trang 5 bài
// // //     auto page1 = library.getPage(1, 5);
// // //     printList(page1, "Page 1 (Size 5)");

// // //     // --- Test 2.2: Sorting ---
// // //     std::cout << "\n--- TEST SORTING ---\n";
// // //     library.sortByArtist(true); // Sắp xếp theo Artist A-Z
// // //     auto sortedPage = library.getPage(1, 25); // Lấy lại trang 1 để xem kết quả
// // //     printList(sortedPage, "Top 5 files sorted by Artist (A-Z)");

// // //     // --- Test 2.3: Searching ---
// // //     std::cout << "\n--- TEST SEARCHING ---\n";
// // //     // Tìm thử chữ "a" (khả năng cao sẽ có nhiều kết quả)
// // //     std::string keyword = "a"; 
// // //     auto searchResults = library.searchByArtist(keyword);
// // //     printList(searchResults, "Search Artist containing '" + keyword + "'");

// // //     // ==========================================
// // //     // PHẦN 3: PLAYBACK HISTORY (Module 2)
// // //     // ==========================================
// // //     std::cout << "\n--- TEST HISTORY ---\n";
// // //     PlaybackHistory history(3); // Chỉ lưu tối đa 3 bài

// // //     if (rawFiles.size() >= 2) {
// // //         std::cout << ">> Playing: " << rawFiles[0].getFileName() << "\n";
// // //         history.add(rawFiles[0]);

// // //         std::cout << ">> Playing: " << rawFiles[1].getFileName() << "\n";
// // //         history.add(rawFiles[1]);

// // //         std::cout << ">> Playing again: " << rawFiles[0].getFileName() << " (Should move to top)\n";
// // //         history.add(rawFiles[0]); // Add trùng lặp -> phải lên đầu
        
// // //         auto recent = history.getRecent(10);
// // //         printList(recent, "Recent History");
// // //     }

// // //     return 0;
// // // }

// // /* Test Module 3 */

// // #include <iostream>
// // #include <vector>
// // #include <memory>
// // #include <iomanip>

// // // Module 1: Scanner
// // #include "service/scanner/LinuxFileSystem.h"
// // #include "service/scanner/FileScanner.h"

// // // Module 2: Library
// // #include "model/MediaLibrary.h"

// // // Module 3: Playlist
// // #include "service/PlaylistManager.h"
// // #include "service/repository/JsonPlaylistRepository.h"

// // // --- Helper: In thông tin Playlist ---
// // void printAllPlaylists(const PlaylistManager& manager) {
// //     const auto& playlists = manager.getAllPlaylists();
// //     std::cout << "\n>>> CURRENT PLAYLISTS (" << playlists.size() << ")\n";
    
// //     if (playlists.empty()) {
// //         std::cout << "    (No playlists created yet)\n";
// //         return;
// //     }

// //     for (const auto& pl : playlists) {
// //         std::cout << "  + [" << pl.getName() << "] - " << pl.size() << " songs\n";
// //         for (const auto& item : pl.getItems()) {
// //             std::cout << "      - " << item.getPath() << "\n";
// //         }
// //     }
// //     std::cout << "--------------------------------------------------\n";
// // }

// // int main() {
// //     // 1. SETUP DATA SOURCE (Module 1)
// //     std::string scanPath = "/media/sf_Playlist/Music";
// //     std::cout << "[1] Scanning music from: " << scanPath << " ...\n";

// //     auto fs = std::make_shared<LinuxFileSystem>();
// //     FileScanner scanner(fs);
// //     std::vector<MediaFile> rawFiles = scanner.scanDirectory(scanPath);

// //     if (rawFiles.empty()) {
// //         std::cout << "[ERROR] No files found! Cannot demo playlist creation.\n";
// //         return 0;
// //     }
// //     std::cout << "    Found " << rawFiles.size() << " files.\n";

// //     // 2. SETUP PLAYLIST MANAGER (Module 3)
// //     // File này sẽ được tạo ra tại thư mục chạy chương trình (thường là build/bin)
// //     std::string dbPath = "playlists.db"; 
    
// //     // Inject Repository vào Manager
// //     JsonPlaylistRepository repo(dbPath);
// //     PlaylistManager plManager(repo);

// //     // 3. LOAD OLD DATA
// //     std::cout << "\n[2] Loading existing playlists from " << dbPath << "...\n";
// //     plManager.load();
// //     printAllPlaylists(plManager);

// //     // 4. CREATE NEW PLAYLIST & ADD SONGS
// //     std::string newPlName = "My Favorites";
// //     std::cout << "\n[3] Creating/Updating playlist: '" << newPlName << "'...\n";
    
// //     // Tạo playlist nếu chưa có
// //     plManager.createPlaylist(newPlName);
// //     Playlist* myFav = plManager.getPlaylist(newPlName);

// //     if (myFav) {
// //         // Thử thêm 3 bài hát đầu tiên vào playlist này
// //         int count = 0;
// //         for (const auto& song : rawFiles) {
// //             if (count >= 3) break;
            
// //             // Logic: Thêm vào playlist
// //             myFav->addMedia(song);
// //             std::cout << "    Added: " << song.getFileName() << "\n";
// //             count++;
// //         }
// //     }

// //     // 5. SAVE TO FILE
// //     std::cout << "\n[4] Saving all playlists to disk...\n";
// //     plManager.save();
// //     std::cout << "    Saved successfully to " << dbPath << "\n";

// //     // 6. VERIFY
// //     std::cout << "\n[5] Final Verification:\n";
// //     printAllPlaylists(plManager);

// //     return 0;
// // }

// /* Test Module 4 */
// #include <iostream>
// #include <vector>
// #include <memory>
// #include <iomanip> // std::setw, std::left

// // Module 1: Scanner
// #include "service/scanner/LinuxFileSystem.h"
// #include "service/scanner/FileScanner.h"

// // Module 4: Metadata Service
// #include "service/metadata/TagLibMetadataService.h"

// // --- Helper: Hàm in dòng kẻ ngang ---
// void printDivider(int width) {
//     std::cout << std::string(width, '-') << "\n";
// }

// int main(int argc, char* argv[]) {
//     // 1. SETUP ĐƯỜNG DẪN (Mặc định hoặc từ tham số)
//     std::string scanPath = "/media/sf_Playlist/Music";
//     if (argc > 1) {
//         scanPath = argv[1];
//     }

//     std::cout << ">>> 1. Scanning directory: " << scanPath << " ...\n";

//     // 2. QUÉT FILE (Module 1)
//     auto fs = std::make_shared<LinuxFileSystem>();
//     FileScanner scanner(fs);
//     std::vector<MediaFile> files = scanner.scanDirectory(scanPath);

//     if (files.empty()) {
//         std::cout << "[!] No media files found. Check path or add some MP3 files.\n";
//         return 0;
//     }
//     std::cout << ">>> Found " << files.size() << " files. Reading tags...\n\n";

//     // 3. ĐỌC METADATA (Module 4)
//     TagLibMetadataService metadataService;
    
//     // In Header Bảng
//     int wTitle = 35;
//     int wArtist = 25;
//     int wAlbum = 25;
//     int wYear = 6;
//     int totalWidth = wTitle + wArtist + wAlbum + wYear + 15;

//     printDivider(totalWidth);
//     std::cout << std::left 
//               << std::setw(wTitle) << "TITLE (Track Name)" 
//               << std::setw(wArtist) << "ARTIST" 
//               << std::setw(wAlbum) << "ALBUM" 
//               << std::setw(wYear) << "YEAR" 
//               << "COVER?" << "\n";
//     printDivider(totalWidth);

//     int successCount = 0;

//     for (auto& file : files) {
//         // Chỉ đọc metadata cho file Audio (Video hỗ trợ hạn chế hơn)
//         if (file.getType() == MediaType::Audio) {
            
//             // --- GỌI TAGLIB ĐỂ ĐỌC DỮ LIỆU THẬT ---
//             bool isReadSuccess = metadataService.readMetadata(file);
            
//             if (isReadSuccess) {
//                 successCount++;
                
//                 // Lấy data đã được nạp vào MediaFile
//                 const auto& meta = file.getMetadata();
                
//                 // Xử lý hiển thị (Cắt chuỗi nếu quá dài)
//                 std::string dTitle = meta.title.empty() ? "[No Title]" : meta.title;
//                 std::string dArtist = meta.artist.empty() ? "[Unknown]" : meta.artist;
//                 std::string dAlbum = meta.album.empty() ? "[Unknown]" : meta.album;
                
//                 if (dTitle.length() > wTitle - 3) dTitle = dTitle.substr(0, wTitle - 3) + "..";
//                 if (dArtist.length() > wArtist - 3) dArtist = dArtist.substr(0, wArtist - 3) + "..";
//                 if (dAlbum.length() > wAlbum - 3) dAlbum = dAlbum.substr(0, wAlbum - 3) + "..";
                
//                 std::string hasCover = meta.coverArtData.empty() ? "No" : "YES";

//                 // In ra màn hình
//                 std::cout << std::left 
//                           << std::setw(wTitle) << dTitle 
//                           << std::setw(wArtist) << dArtist 
//                           << std::setw(wAlbum) << dAlbum 
//                           << std::setw(wYear) << meta.year 
//                           << hasCover << "\n";
//             } else {
//                 std::cout << "[!] Failed to read tags: " << file.getFileName() << "\n";
//             }
//         } else {
//             // File Video hoặc file khác
//             std::cout << std::left << std::setw(wTitle) << ("[" + file.getFileName() + "]") 
//                       << " (Video/Other format skipped for now)\n";
//         }
//     }

//     printDivider(totalWidth);
//     std::cout << "Summary: Successfully read tags for " << successCount << "/" << files.size() << " files.\n";

//     return 0;
// }

/* Test PlaybackController */

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <iomanip>

// Module 1: Scanner
#include "service/scanner/LinuxFileSystem.h"
#include "service/scanner/FileScanner.h"

// Module 5: Playback Engine
#include "service/player/SDLAudioPlayer.h"
#include "controller/PlaybackController.h"

void clearScreen() {
    // Hack nhỏ để xóa màn hình console linux
    std::cout << "\033[2J\033[1;1H"; 
}

void printStatus(const PlaybackController& controller) {
    clearScreen();
    std::cout << "========================================\n";
    std::cout << "   SIMPLE CONSOLE MUSIC PLAYER (C++17)  \n";
    std::cout << "========================================\n";

    const MediaFile* current = controller.getCurrentMedia();
    if (current) {
        std::cout << "NP: " << current->getFileName() << "\n";
        
        // Thời gian: 00:15 / 03:40
        int cur = controller.getCurrentTime();
        int dur = controller.getDuration();
        std::cout << "Time: " << (cur / 60) << ":" << std::setw(2) << std::setfill('0') << (cur % 60)
                  << " / " << (dur / 60) << ":" << std::setw(2) << std::setfill('0') << (dur % 60) << "\n";
        
        std::cout << "State: " << (controller.isPlaying() ? "[PLAYING]" : "[PAUSED]") << "\n";
    } else {
        std::cout << "NP: [Stopped / No File]\n";
    }

    std::cout << "----------------------------------------\n";
    std::cout << "Controls:\n";
    std::cout << "  [n] Next       [p] Previous\n";
    std::cout << "  [space] Pause  [s] Stop\n";
    std::cout << "  [+] Vol Up     [-] Vol Down\n";
    std::cout << "  [r] Replay     [q] Quit\n";
    std::cout << "----------------------------------------\n";
    std::cout << "Enter command: ";
}

int main(int argc, char* argv[]) {
    // 1. SETUP DATA (Quét nhạc)
    std::string scanPath = "/home/duong/Music";
    if (argc > 1) scanPath = argv[1];

    std::cout << "Scanning: " << scanPath << "...\n";
    auto fs = std::make_shared<LinuxFileSystem>();
    FileScanner scanner(fs);
    std::vector<MediaFile> playlist = scanner.scanDirectory(scanPath);

    if (playlist.empty()) {
        std::cerr << "Error: No music files found in " << scanPath << "\n";
        return 1;
    }

    // 2. SETUP AUDIO ENGINE
    // Tạo Player Implementation (SDL + FFmpeg)
    SDLAudioPlayer sdlPlayer;
    
    // Tạo Controller và bơm Player vào (Dependency Injection)
    PlaybackController controller(sdlPlayer);

    // 3. LOAD PLAYLIST & PLAY
    controller.setPlaylist(playlist);
    controller.setVolume(50); // Mặc định 50%
    controller.play(); // Bắt đầu phát bài đầu tiên

    // 4. MAIN LOOP (Giả lập UI Event Loop)
    char cmd;
    bool running = true;

    while (running) {
        printStatus(controller);
        
        // Chờ người dùng nhập ký tự
        std::cin >> cmd;

        switch (cmd) {
            case 'n': controller.next(); break;
            case 'p': controller.previous(); break;
            case 's': controller.stop(); break;
            case ' ': // Space không hoạt động tốt với cin, dùng tạm 't' toggle hoặc nhập ký tự khác
            case 't': controller.pause(); break; 
            case '+': controller.setVolume(80); break; // Demo set cứng mức volume
            case '-': controller.setVolume(20); break;
            case 'r': controller.playIndex(0); break; // Replay list
            case 'q': running = false; break;
            default: break;
        }

        // Ngủ một chút để thread audio xử lý kịp trước khi vẽ lại UI
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    controller.stop();
    return 0;
}
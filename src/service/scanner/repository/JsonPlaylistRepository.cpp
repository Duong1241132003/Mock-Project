#include "service/repository/JsonPlaylistRepository.h"
#include <fstream>
#include <iostream>
#include <sstream>

JsonPlaylistRepository::JsonPlaylistRepository(std::string filePath)
    : m_filePath(std::move(filePath)) {}

void JsonPlaylistRepository::save(const std::vector<Playlist>& playlists) {
    std::ofstream outFile(m_filePath);
    if (!outFile.is_open()) {
        std::cerr << "[Error] Cannot open file for writing: " << m_filePath << "\n";
        return;
    }

    // Simple Custom Format:
    // #PlaylistName
    // /path/to/file1
    // /path/to/file2
    for (const auto& pl : playlists) {
        outFile << "#" << pl.getName() << "\n";
        for (const auto& item : pl.getItems()) {
            outFile << item.getPath() << "\n";
        }
    }
    outFile.close();
}

std::vector<Playlist> JsonPlaylistRepository::load() {
    std::vector<Playlist> playlists;
    std::ifstream inFile(m_filePath);
    
    if (!inFile.is_open()) {
        // File might not exist yet (first run), just return empty
        return playlists; 
    }

    std::string line;
    Playlist* currentPlaylist = nullptr;

    while (std::getline(inFile, line)) {
        if (line.empty()) continue;

        if (line[0] == '#') {
            // New Playlist started
            std::string name = line.substr(1);
            playlists.emplace_back(name);
            // Get pointer to the last added playlist
            currentPlaylist = &playlists.back(); 
        } else {
            // It's a file path
            if (currentPlaylist) {
                // Reconstruct MediaFile from path
                // Note: Metadata (Artist/Album) is lost in this simple format
                // In a real app, you would re-scan or save metadata in JSON
                MediaFile media(line, "UnknownFile", MediaType::Unknown); 
                currentPlaylist->addMedia(media);
            }
        }
    }
    
    inFile.close();
    return playlists;
}
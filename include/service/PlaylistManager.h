#ifndef SERVICE_PLAYLIST_MANAGER_H
#define SERVICE_PLAYLIST_MANAGER_H

#include <vector>
#include <string>
#include <memory>
#include "model/Playlist.h"
#include "service/repository/IPlaylistRepository.h"

/**
 * @brief Manages the lifecycle of playlists.
 * Logic only - depends on abstraction (IPlaylistRepository).
 */
class PlaylistManager {
public:
    // Dependency Injection via Constructor
    explicit PlaylistManager(IPlaylistRepository& repository);

    // CRUD Operations
    void createPlaylist(const std::string& name);
    void deletePlaylist(const std::string& name);
    void renamePlaylist(const std::string& oldName, const std::string& newName);

    // Retrieval
    // Returns nullptr if not found
    Playlist* getPlaylist(const std::string& name);
    [[nodiscard]] const std::vector<Playlist>& getAllPlaylists() const;

    // Persistence Delegation
    void save();
    void load();

private:
    std::vector<Playlist> m_playlists;
    IPlaylistRepository& m_repository; // Reference to the repository

    // Helper to find iterator
    std::vector<Playlist>::iterator findPlaylistIterator(const std::string& name);
};

#endif // SERVICE_PLAYLIST_MANAGER_H
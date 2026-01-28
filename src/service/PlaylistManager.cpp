#include "service/PlaylistManager.h"
#include <algorithm>

PlaylistManager::PlaylistManager(IPlaylistRepository& repository)
    : m_repository(repository) {}

void PlaylistManager::createPlaylist(const std::string& name) {
    // Check if exists
    if (getPlaylist(name) != nullptr) {
        return; // Already exists, do nothing (or throw exception)
    }
    m_playlists.emplace_back(name);
}

void PlaylistManager::deletePlaylist(const std::string& name) {
    auto it = findPlaylistIterator(name);
    if (it != m_playlists.end()) {
        m_playlists.erase(it);
    }
}

void PlaylistManager::renamePlaylist(const std::string& oldName, const std::string& newName) {
    Playlist* pl = getPlaylist(oldName);
    if (pl) {
        // Also ensure newName doesn't exist
        if (getPlaylist(newName) == nullptr) {
            pl->setName(newName);
        }
    }
}

Playlist* PlaylistManager::getPlaylist(const std::string& name) {
    auto it = findPlaylistIterator(name);
    if (it != m_playlists.end()) {
        return &(*it);
    }
    return nullptr;
}

const std::vector<Playlist>& PlaylistManager::getAllPlaylists() const {
    return m_playlists;
}

void PlaylistManager::save() {
    m_repository.save(m_playlists);
}

void PlaylistManager::load() {
    m_playlists = m_repository.load();
}

std::vector<Playlist>::iterator PlaylistManager::findPlaylistIterator(const std::string& name) {
    return std::find_if(m_playlists.begin(), m_playlists.end(),
        [&name](const Playlist& p) {
            return p.getName() == name;
        });
}
#ifndef SERVICE_REPOSITORY_IPLAYLISTREPOSITORY_H
#define SERVICE_REPOSITORY_IPLAYLISTREPOSITORY_H

#include <vector>
#include "model/Playlist.h"

/**
 * @brief Interface for playlist persistence.
 * Adheres to Dependency Inversion Principle.
 */
class IPlaylistRepository {
public:
    virtual ~IPlaylistRepository() = default;

    // Saves all playlists to the storage medium
    virtual void save(const std::vector<Playlist>& playlists) = 0;

    // Loads all playlists from the storage medium
    virtual std::vector<Playlist> load() = 0;
};

#endif // SERVICE_REPOSITORY_IPLAYLISTREPOSITORY_H
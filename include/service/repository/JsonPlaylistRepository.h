#ifndef SERVICE_REPOSITORY_JSON_IMPL_H
#define SERVICE_REPOSITORY_JSON_IMPL_H

#include "IPlaylistRepository.h"
#include <string>

/**
 * @brief Concrete implementation of storage using a simple text-based format.
 * (Named 'Json' as per request, but implements a simplified parser for standard C++ compatibility).
 */
class JsonPlaylistRepository : public IPlaylistRepository {
public:
    explicit JsonPlaylistRepository(std::string filePath);

    void save(const std::vector<Playlist>& playlists) override;
    std::vector<Playlist> load() override;

private:
    std::string m_filePath;
};

#endif // SERVICE_REPOSITORY_JSON_IMPL_H
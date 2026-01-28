#ifndef MODEL_MEDIA_LIBRARY_H
#define MODEL_MEDIA_LIBRARY_H

#include <vector>
#include <string>
#include <functional> 
#include "MediaFile.h"

/**
 * @brief Manages the collection of media files.
 * Responsibilities: Storage, Pagination, Searching, Sorting.
 * Logic only - No file system access.
 */
class MediaLibrary {
public:
    MediaLibrary() = default;

    // Load data into the library
    void setMediaList(const std::vector<MediaFile>& mediaList);

    // --- Pagination ---
    // Page number starts from 1.
    [[nodiscard]] std::vector<MediaFile> getPage(size_t pageNumber, size_t pageSize) const;
    [[nodiscard]] size_t getTotalMediaCount() const;

    // --- Searching ---
    // Returns a subset of media files matching the keyword (case-insensitive substring)
    [[nodiscard]] std::vector<MediaFile> searchByTitle(const std::string& keyword) const;
    [[nodiscard]] std::vector<MediaFile> searchByArtist(const std::string& keyword) const;
    [[nodiscard]] std::vector<MediaFile> searchByAlbum(const std::string& keyword) const;

    // --- Sorting ---
    // Sorts the INTERNAL list. Subsequent calls to getPage will reflect the new order.
    void sortByTitle(bool ascending = true);
    void sortByArtist(bool ascending = true);
    void sortByAlbum(bool ascending = true);

private:
    std::vector<MediaFile> m_mediaList;

    // Internal helper for generic search to reduce code duplication
    std::vector<MediaFile> search(const std::string& keyword, 
        std::function<std::string(const MediaFile&)> fieldExtractor) const;
    
    // Internal helper for string comparison logic
    bool containsIgnoreCase(const std::string& source, const std::string& keyword) const;
};

#endif // MODEL_MEDIA_LIBRARY_H
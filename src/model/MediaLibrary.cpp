#include "model/MediaLibrary.h"
#include <algorithm> // sort, search, transform
#include <cctype>    // tolower
#include <cmath>     // min

void MediaLibrary::setMediaList(const std::vector<MediaFile>& mediaList) {
    m_mediaList = mediaList;
}

size_t MediaLibrary::getTotalMediaCount() const {
    return m_mediaList.size();
}

std::vector<MediaFile> MediaLibrary::getPage(size_t pageNumber, size_t pageSize) const {
    std::vector<MediaFile> pageItems;
    
    if (pageNumber == 0 || pageSize == 0 || m_mediaList.empty()) {
        return pageItems;
    }

    size_t startIndex = (pageNumber - 1) * pageSize;
    
    if (startIndex >= m_mediaList.size()) {
        return pageItems; // Page out of range
    }

    size_t endIndex = std::min(startIndex + pageSize, m_mediaList.size());
    
    // Copy range to result vector
    for (size_t i = startIndex; i < endIndex; ++i) {
        pageItems.push_back(m_mediaList[i]);
    }

    return pageItems;
}

// --- Helper Functions ---

bool MediaLibrary::containsIgnoreCase(const std::string& source, const std::string& keyword) const {
    if (keyword.empty()) return true; // Empty keyword matches everything
    
    auto it = std::search(
        source.begin(), source.end(),
        keyword.begin(), keyword.end(),
        [](char ch1, char ch2) {
            return std::tolower(static_cast<unsigned char>(ch1)) ==
                   std::tolower(static_cast<unsigned char>(ch2));
        }
    );
    return it != source.end();
}

std::vector<MediaFile> MediaLibrary::search(const std::string& keyword, 
                                            std::function<std::string(const MediaFile&)> fieldExtractor) const {
    std::vector<MediaFile> results;
    for (const auto& media : m_mediaList) {
        if (containsIgnoreCase(fieldExtractor(media), keyword)) {
            results.push_back(media);
        }
    }
    return results;
}

// --- Searching Implementations ---

std::vector<MediaFile> MediaLibrary::searchByTitle(const std::string& keyword) const {
    return search(keyword, [](const MediaFile& m) { return m.getTitle(); });
}

std::vector<MediaFile> MediaLibrary::searchByArtist(const std::string& keyword) const {
    return search(keyword, [](const MediaFile& m) { return m.getArtist(); });
}

std::vector<MediaFile> MediaLibrary::searchByAlbum(const std::string& keyword) const {
    return search(keyword, [](const MediaFile& m) { return m.getAlbum(); });
}

// --- Sorting Implementations ---

void MediaLibrary::sortByTitle(bool ascending) {
    std::sort(m_mediaList.begin(), m_mediaList.end(), 
        [ascending](const MediaFile& a, const MediaFile& b) {
            return ascending ? a.getTitle() < b.getTitle() 
                             : a.getTitle() > b.getTitle();
        });
}

void MediaLibrary::sortByArtist(bool ascending) {
    std::sort(m_mediaList.begin(), m_mediaList.end(), 
        [ascending](const MediaFile& a, const MediaFile& b) {
            return ascending ? a.getArtist() < b.getArtist() 
                             : a.getArtist() > b.getArtist();
        });
}

void MediaLibrary::sortByAlbum(bool ascending) {
    std::sort(m_mediaList.begin(), m_mediaList.end(), 
        [ascending](const MediaFile& a, const MediaFile& b) {
            return ascending ? a.getAlbum() < b.getAlbum() 
                             : a.getAlbum() > b.getAlbum();
        });
}
#ifndef MODEL_PLAYLIST_H
#define MODEL_PLAYLIST_H

#include <string>
#include <vector>
#include <algorithm>
#include "MediaFile.h"

/**
 * @brief Represents a single playlist containing a name and a list of media files.
 * Pure Model class.
 */
class Playlist {
public:
    explicit Playlist(std::string name);

    // Getters & Setters
    [[nodiscard]] const std::string& getName() const;
    void setName(const std::string& newName);

    // List Management
    void addMedia(const MediaFile& media);
    void removeMedia(const std::string& mediaPath);
    
    [[nodiscard]] const std::vector<MediaFile>& getItems() const;
    [[nodiscard]] size_t size() const;

private:
    std::string m_name;
    std::vector<MediaFile> m_items;
};

#endif // MODEL_PLAYLIST_H
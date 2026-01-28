#include "model/Playlist.h"

Playlist::Playlist(std::string name) : m_name(std::move(name)) {}

const std::string& Playlist::getName() const {
    return m_name;
}

void Playlist::setName(const std::string& newName) {
    m_name = newName;
}

void Playlist::addMedia(const MediaFile& media) {
    // Optional: Check for duplicates before adding if needed
    m_items.push_back(media);
}

void Playlist::removeMedia(const std::string& mediaPath) {
    auto it = std::remove_if(m_items.begin(), m_items.end(),
        [&mediaPath](const MediaFile& item) {
            return item.getPath() == mediaPath;
        });
    
    if (it != m_items.end()) {
        m_items.erase(it, m_items.end());
    }
}

const std::vector<MediaFile>& Playlist::getItems() const {
    return m_items;
}

size_t Playlist::size() const {
    return m_items.size();
}
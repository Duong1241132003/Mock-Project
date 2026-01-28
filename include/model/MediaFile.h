#ifndef MODEL_MEDIA_FILE_H
#define MODEL_MEDIA_FILE_H

#include <string>
#include "MediaMetadata.h"

enum class MediaType { Audio, Video, Unknown };

class MediaFile {
public:
    MediaFile(std::string path, std::string fileName, MediaType type)
        : m_path(std::move(path)), m_fileName(std::move(fileName)), m_type(type) 
    {
        // Default fallback
        m_metadata.title = m_fileName;
        m_metadata.artist = "Unknown Artist";
        m_metadata.album = "Unknown Album";
    }

    // --- Basic Info ---
    [[nodiscard]] const std::string& getPath() const { return m_path; }
    [[nodiscard]] const std::string& getFileName() const { return m_fileName; }
    [[nodiscard]] MediaType getType() const { return m_type; }

    // --- Metadata Accessors (Proxy to struct) ---
    [[nodiscard]] const MediaMetadata& getMetadata() const { return m_metadata; }
    MediaMetadata& getMetadata() { return m_metadata; } // Mutable accessor for Service
    
    void setMetadata(const MediaMetadata& meta) { m_metadata = meta; }

    // Compatibility Getters (Module 2)
    [[nodiscard]] const std::string& getTitle() const { return m_metadata.title; }
    [[nodiscard]] const std::string& getArtist() const { return m_metadata.artist; }
    [[nodiscard]] const std::string& getAlbum() const { return m_metadata.album; }

    // Compatibility Setters
    void setTitle(const std::string& t) { m_metadata.title = t; }
    void setArtist(const std::string& a) { m_metadata.artist = a; }
    void setAlbum(const std::string& a) { m_metadata.album = a; }

private:
    std::string m_path;
    std::string m_fileName;
    MediaType m_type;
    
    // Embedded Metadata Struct
    MediaMetadata m_metadata;
};

#endif // MODEL_MEDIA_FILE_H
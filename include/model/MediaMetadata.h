#ifndef MODEL_MEDIA_METADATA_H
#define MODEL_MEDIA_METADATA_H

#include <string>
#include <vector>

/**
 * @brief Pure data structure holding detailed information about a media file.
 * Independent of any library (TagLib, FFmpeg).
 */
struct MediaMetadata {
    std::string title;
    std::string artist;
    std::string album;
    std::string genre;
    unsigned int year = 0;
    int durationSeconds = 0;
    int bitrate = 0; // kbps
    
    // Cover Art data (Raw bytes of the image: JPEG/PNG)
    std::vector<unsigned char> coverArtData; 
    
    // Helper to check if metadata is effectively empty
    bool isEmpty() const {
        return title.empty() && artist.empty();
    }
};

#endif // MODEL_MEDIA_METADATA_H
#include "service/metadata/TagLibMetadataService.h"

// TagLib Includes
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>

#include <iostream>

bool TagLibMetadataService::readMetadata(MediaFile& media) {
    TagLib::FileRef f(media.getPath().c_str());

    if (f.isNull() || !f.tag()) {
        return false; 
    }

    TagLib::Tag *tag = f.tag();
    MediaMetadata& meta = media.getMetadata();

    // 1. Basic Tags (Universal)
    // to8Bit(true) forces UTF-8 encoding
    meta.title = tag->title().to8Bit(true);
    meta.artist = tag->artist().to8Bit(true);
    meta.album = tag->album().to8Bit(true);
    meta.genre = tag->genre().to8Bit(true);
    meta.year = tag->year();

    // 2. Audio Properties
    if (f.audioProperties()) {
        meta.durationSeconds = f.audioProperties()->length();
        meta.bitrate = f.audioProperties()->bitrate();
    }

    // Fallback if tags are empty, use filename
    if (meta.title.empty()) {
        meta.title = media.getFileName();
    }

    // 3. Extract Cover Art (Specific handling for MP3/ID3v2)
    // Note: Video thumbnails usually require FFmpeg, TagLib is mainly for Audio covers
    if (media.getType() == MediaType::Audio) {
        extractCoverArt(media.getPath(), meta);
    }

    return true;
}

bool TagLibMetadataService::writeMetadata(const MediaFile& media) {
    TagLib::FileRef f(media.getPath().c_str());

    if (f.isNull() || !f.tag()) {
        return false;
    }

    TagLib::Tag *tag = f.tag();
    const MediaMetadata& meta = media.getMetadata();

    // Update tags
    tag->setTitle(TagLib::String(meta.title, TagLib::String::UTF8));
    tag->setArtist(TagLib::String(meta.artist, TagLib::String::UTF8));
    tag->setAlbum(TagLib::String(meta.album, TagLib::String::UTF8));
    tag->setGenre(TagLib::String(meta.genre, TagLib::String::UTF8));
    tag->setYear(meta.year);

    return f.save();
}

void TagLibMetadataService::extractCoverArt(const std::string& path, MediaMetadata& outMeta) {
    // Attempt to treat it as an MPEG (MP3) file to get ID3v2 tags
    TagLib::MPEG::File mpegFile(path.c_str());
    
    if (!mpegFile.isValid()) return;

    if (mpegFile.ID3v2Tag()) {
        TagLib::ID3v2::Tag *id3v2tag = mpegFile.ID3v2Tag();
        // Look for picture frames
        auto frameList = id3v2tag->frameListMap()["APIC"];
        
        if (!frameList.isEmpty()) {
            // Get the first picture frame
            auto *frame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(frameList.front());
            
            if (frame) {
                TagLib::ByteVector pictureData = frame->picture();
                // Copy data to our vector
                outMeta.coverArtData.assign(pictureData.data(), pictureData.data() + pictureData.size());
            }
        }
    }
}
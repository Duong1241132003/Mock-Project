#ifndef SERVICE_METADATA_TAGLIB_IMPL_H
#define SERVICE_METADATA_TAGLIB_IMPL_H

#include "IMetadataService.h"

/**
 * @brief Implementation of IMetadataService using TagLib.
 * This class handles the mapping between TagLib types and our MediaMetadata struct.
 */
class TagLibMetadataService : public IMetadataService {
public:
    bool readMetadata(MediaFile& media) override;
    bool writeMetadata(const MediaFile& media) override;

private:
    // Helper to extract specific ID3v2 Cover Art (Complex logic hidden here)
    void extractCoverArt(const std::string& path, MediaMetadata& outMeta);
};

#endif // SERVICE_METADATA_TAGLIB_IMPL_H
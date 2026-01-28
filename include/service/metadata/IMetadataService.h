#ifndef SERVICE_METADATA_IMETADATASERVICE_H
#define SERVICE_METADATA_IMETADATASERVICE_H

#include "model/MediaFile.h"

/**
 * @brief Interface for reading/writing media metadata.
 * Follows Dependency Inversion Principle.
 */
class IMetadataService {
public:
    virtual ~IMetadataService() = default;

    /**
     * @brief Reads metadata from the physical file and updates the MediaFile object.
     * @return true if successful, false otherwise.
     */
    virtual bool readMetadata(MediaFile& media) = 0;

    /**
     * @brief Writes current metadata in MediaFile back to the physical file.
     * @return true if successful, false otherwise.
     */
    virtual bool writeMetadata(const MediaFile& media) = 0;
};

#endif // SERVICE_METADATA_IMETADATASERVICE_H
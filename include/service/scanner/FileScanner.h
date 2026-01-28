#ifndef FILESCANNER_H
#define FILESCANNER_H

#include "service/scanner/IFileSystem.h"
#include "model/MediaFile.h"
#include <vector>
#include <memory>
#include <unordered_set>

/**
 * @brief Responsible for scanning directories and filtering media files.
 * Implements strict logic separation: No direct OS calls.
 */
class FileScanner {
public:
    // Dependency Injection via Constructor
    explicit FileScanner(std::shared_ptr<IFileSystem> fileSystem);

    /**
     * @brief Recursively scans the directory and returns valid media files.
     * @param rootPath The starting directory path.
     * @return A vector of MediaFile objects.
     */
    std::vector<MediaFile> scanDirectory(const std::string& rootPath);

private:
    std::shared_ptr<IFileSystem> m_fileSystem;

    // Supported extensions (Constant for OCP support - easy to extend)
    const std::unordered_set<std::string> AUDIO_EXTENSIONS = { ".mp3", ".wav", ".ogg" };
    const std::unordered_set<std::string> VIDEO_EXTENSIONS = { ".mp4", ".mkv" };

    // Helper: Determine file type based on extension string
    MediaType determineMediaType(const std::string& fileName) const;

    // Helper: extract extension from string (std::filesystem forbidden here)
    std::string getFileExtension(const std::string& fileName) const;
    
    // Helper: extract filename from full path
    std::string getFileNameFromPath(const std::string& path) const;
};

#endif // FILESCANNER_H 

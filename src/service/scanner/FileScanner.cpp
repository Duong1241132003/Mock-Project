#include "service/scanner/FileScanner.h"
#include <algorithm>
#include <cctype>

FileScanner::FileScanner(std::shared_ptr<IFileSystem> fileSystem)
    : m_fileSystem(std::move(fileSystem)) {
}

std::vector<MediaFile> FileScanner::scanDirectory(const std::string& rootPath) {
    std::vector<MediaFile> results;

    // Guard: Path must exist
    if (!m_fileSystem->exists(rootPath)) {
        return results;
    }

    // Get all entries in the current directory
    std::vector<std::string> entries = m_fileSystem->listEntries(rootPath);

    for (const auto& entryPath : entries) {
        // Guard: Skip symbolic links to avoid infinite recursion loops
        if (m_fileSystem->isSymlink(entryPath)) {
            continue;
        }

        if (m_fileSystem->isDirectory(entryPath)) {
            // RECURSION: Scan subdirectory and merge results
            std::vector<MediaFile> subResults = scanDirectory(entryPath);
            results.insert(results.end(), subResults.begin(), subResults.end());
        } else {
            // FILE HANDLING: Check extension and filter
            std::string fileName = getFileNameFromPath(entryPath);
            MediaType type = determineMediaType(fileName);

            if (type != MediaType::Unknown) {
                results.emplace_back(entryPath, fileName, type);
            }
        }
    }

    return results;
}

MediaType FileScanner::determineMediaType(const std::string& fileName) const {
    std::string ext = getFileExtension(fileName);
    
    // Check against Audio set
    if (AUDIO_EXTENSIONS.find(ext) != AUDIO_EXTENSIONS.end()) {
        return MediaType::Audio;
    }
    
    // Check against Video set
    if (VIDEO_EXTENSIONS.find(ext) != VIDEO_EXTENSIONS.end()) {
        return MediaType::Video;
    }

    return MediaType::Unknown;
}

std::string FileScanner::getFileExtension(const std::string& fileName) const {
    size_t dotPos = fileName.rfind('.');
    if (dotPos == std::string::npos) {
        return "";
    }

    std::string ext = fileName.substr(dotPos);
    
    // Convert to lowercase to ensure case-insensitive matching
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    
    return ext;
}

std::string FileScanner::getFileNameFromPath(const std::string& path) const {
    // Manually find the last slash to separate filename from path
    // Handles both Linux forward slash and Windows backslash generic cases
    size_t slashPos = path.find_last_of("/\\");
    if (slashPos != std::string::npos) {
        return path.substr(slashPos + 1);
    }
    return path;
}
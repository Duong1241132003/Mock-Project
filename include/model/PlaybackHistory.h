#ifndef MODEL_PLAYBACK_HISTORY_H
#define MODEL_PLAYBACK_HISTORY_H

#include <deque>
#include <vector>
#include "MediaFile.h"

/**
 * @brief Manages a list of recently played files.
 * Enforces a maximum size limit (FIFO logic with update-to-top).
 */
class PlaybackHistory {
public:
    explicit PlaybackHistory(size_t maxSize = 500);

    // Adds a media file to history.
    // If it exists, moves it to the top.
    // If list exceeds max size, removes oldest.
    void add(const MediaFile& media);

    // Get N most recent items
    [[nodiscard]] std::vector<MediaFile> getRecent(size_t count) const;

    void clear();
    [[nodiscard]] size_t size() const;

private:
    std::deque<MediaFile> m_historyQueue;
    size_t m_maxSize;
};

#endif // MODEL_PLAYBACK_HISTORY_H
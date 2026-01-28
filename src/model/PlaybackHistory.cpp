#include "model/PlaybackHistory.h"
#include <algorithm>

PlaybackHistory::PlaybackHistory(size_t maxSize) : m_maxSize(maxSize) {}

void PlaybackHistory::add(const MediaFile& media) {
    // 1. Remove if already exists (to move it to front later)
    // We compare by Path as the unique identifier
    auto it = std::remove_if(m_historyQueue.begin(), m_historyQueue.end(),
        [&media](const MediaFile& item) {
            return item.getPath() == media.getPath();
        });
    
    if (it != m_historyQueue.end()) {
        m_historyQueue.erase(it, m_historyQueue.end());
    }

    // 2. Add to front
    m_historyQueue.push_front(media);

    // 3. Trim if exceeds max size
    if (m_historyQueue.size() > m_maxSize) {
        m_historyQueue.pop_back();
    }
}

std::vector<MediaFile> PlaybackHistory::getRecent(size_t count) const {
    std::vector<MediaFile> result;
    size_t limit = std::min(count, m_historyQueue.size());

    for (size_t i = 0; i < limit; ++i) {
        result.push_back(m_historyQueue[i]);
    }
    return result;
}

void PlaybackHistory::clear() {
    m_historyQueue.clear();
}

size_t PlaybackHistory::size() const {
    return m_historyQueue.size();
}
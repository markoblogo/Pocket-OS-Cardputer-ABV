#include "persistence.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <unistd.h>

namespace abvx {
namespace {

void setError(std::string* err, const char* stage)
{
    if (err) *err = std::string(stage) + ": " + std::to_string(errno);
}

void keepNewest(std::vector<std::string>* events)
{
    if (events->size() <= kEventLogMaxEntries) return;
    events->erase(events->begin(), events->end() - kEventLogMaxEntries);
}

}  // namespace

bool loadEventLog(const char* path, std::vector<std::string>* events, std::string* err)
{
    if (!path || !events) {
        errno = EINVAL;
        setError(err, "args");
        return false;
    }
    events->clear();
    FILE* file = std::fopen(path, "rb");
    if (!file) {
        if (errno == ENOENT) return true;
        setError(err, "open");
        return false;
    }
    char line[192];
    while (std::fgets(line, sizeof(line), file)) {
        size_t len = std::strlen(line);
        while (len && (line[len - 1] == '\n' || line[len - 1] == '\r')) line[--len] = '\0';
        if (len) events->emplace_back(line, len);
        keepNewest(events);
    }
    const bool read_ok = !std::ferror(file);
    const int read_errno = errno;
    const bool close_ok = std::fclose(file) == 0;
    if (!read_ok || !close_ok) {
        errno = read_errno ? read_errno : EIO;
        setError(err, read_ok ? "close" : "read");
        return false;
    }
    return true;
}

bool mergeEventLog(const char* path, const char* temp_path,
                   const std::vector<std::string>& pending,
                   std::vector<std::string>* merged, std::string* err)
{
    if (!path || !temp_path || !merged) {
        errno = EINVAL;
        setError(err, "args");
        return false;
    }
    if (!loadEventLog(path, merged, err)) return false;
    merged->insert(merged->end(), pending.begin(), pending.end());
    keepNewest(merged);

    FILE* file = std::fopen(temp_path, "wb");
    if (!file) {
        setError(err, "temp open");
        return false;
    }
    bool ok = true;
    for (const std::string& event : *merged) {
        if (std::fwrite(event.data(), 1, event.size(), file) != event.size() ||
            std::fwrite("\n", 1, 1, file) != 1) {
            ok = false;
            break;
        }
    }
    if (ok && std::fflush(file) != 0) ok = false;
    if (ok && fsync(fileno(file)) != 0) ok = false;
    if (std::fclose(file) != 0) ok = false;
    if (!ok) {
        setError(err, "temp write");
        std::remove(temp_path);
        return false;
    }
    // SPIFFS does not reliably replace an existing destination on rename.
    // The complete, synced temp file is already available at this point.
    if (std::remove(path) != 0 && errno != ENOENT) {
        setError(err, "replace");
        std::remove(temp_path);
        return false;
    }
    if (std::rename(temp_path, path) != 0) {
        setError(err, "rename");
        std::remove(temp_path);
        return false;
    }
    return true;
}

}  // namespace abvx

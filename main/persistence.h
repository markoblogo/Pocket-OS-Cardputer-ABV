#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace abvx {

constexpr size_t kEventLogMaxEntries = 64;

// POSIX-only event log helpers. The caller owns filesystem mounting and
// decides when writes are safe relative to audio and removable SD activity.
bool loadEventLog(const char* path, std::vector<std::string>* events, std::string* err = nullptr);
bool mergeEventLog(const char* path, const char* temp_path,
                   const std::vector<std::string>& pending,
                   std::vector<std::string>* merged,
                   std::string* err = nullptr);

}  // namespace abvx

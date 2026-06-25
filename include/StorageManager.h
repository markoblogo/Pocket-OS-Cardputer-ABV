#pragma once

#include <Arduino.h>
#include <FS.h>

class StorageManager {
public:
  bool begin();
  bool ready() const { return ready_; }
  void ensureFolders();
  File open(const char* path, const char* mode);
  bool exists(const char* path) const;
  bool writeText(const char* path, const String& value);
  String readText(const char* path, size_t maxBytes = 8192);
  void log(const String& message);
  uint16_t listFiles(const char* dir, const char* ext, String* out, uint16_t maxItems);
  String nextNumberedPath(const char* dir, const char* prefix, const char* ext);

private:
  bool ready_ = false;
};

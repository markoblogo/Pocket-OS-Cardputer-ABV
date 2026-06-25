#include "StorageManager.h"

#include <SD.h>
#include <SPI.h>

bool StorageManager::begin() {
  ready_ = SD.begin(GPIO_NUM_12, SPI, 25000000);
  if (ready_) ensureFolders();
  return ready_;
}

void StorageManager::ensureFolders() {
  if (!ready_) return;
  const char* dirs[] = {"/music", "/recordings", "/notes", "/books", "/browser",
                        "/browser/bookmarks", "/browser/saved_pages", "/ai",
                        "/config", "/logs", "/tmp"};
  for (auto dir : dirs) {
    if (!SD.exists(dir)) SD.mkdir(dir);
  }
}

File StorageManager::open(const char* path, const char* mode) {
  if (!ready_) return File();
  return SD.open(path, mode);
}

bool StorageManager::exists(const char* path) const {
  return ready_ && SD.exists(path);
}

bool StorageManager::writeText(const char* path, const String& value) {
  if (ready_ && SD.exists(path)) SD.remove(path);
  File f = open(path, FILE_WRITE);
  if (!f) return false;
  f.print(value);
  f.close();
  return true;
}

String StorageManager::readText(const char* path, size_t maxBytes) {
  File f = open(path, FILE_READ);
  if (!f) return "";
  String out;
  while (f.available() && out.length() < maxBytes) out += static_cast<char>(f.read());
  f.close();
  return out;
}

void StorageManager::log(const String& message) {
  if (!ready_) return;
  File f = SD.open("/logs/system.log", FILE_APPEND);
  if (!f) return;
  f.printf("[%lu] %s\n", millis(), message.c_str());
  f.close();
}

uint16_t StorageManager::listFiles(const char* dir, const char* ext, String* out, uint16_t maxItems) {
  if (!ready_) return 0;
  File root = SD.open(dir);
  if (!root || !root.isDirectory()) return 0;
  uint16_t count = 0;
  for (File file = root.openNextFile(); file && count < maxItems; file = root.openNextFile()) {
    String name = file.name();
    if (!file.isDirectory() && (!ext || name.endsWith(ext))) out[count++] = name;
  }
  root.close();
  return count;
}

String StorageManager::nextNumberedPath(const char* dir, const char* prefix, const char* ext) {
  for (uint16_t i = 1; i < 10000; ++i) {
    String path = String(dir) + "/" + prefix + String(i) + ext;
    if (!exists(path.c_str())) return path;
  }
  return String(dir) + "/" + prefix + "9999" + ext;
}

#pragma once

#include <Arduino.h>

class StorageManager;

class FileBrowser {
public:
  void begin(StorageManager& storage, const char* dir, const char* ext);
  void refresh();
  void move(int delta);
  String selectedPath() const;
  String selectedName() const;
  uint16_t count() const { return count_; }
  uint16_t selected() const { return selected_; }
  uint16_t top() const { return top_; }
  String item(uint16_t index) const { return index < count_ ? items_[index] : ""; }

private:
  static constexpr uint16_t MaxItems = 64;
  StorageManager* storage_ = nullptr;
  const char* dir_ = "/";
  const char* ext_ = nullptr;
  String items_[MaxItems];
  uint16_t count_ = 0;
  uint16_t selected_ = 0;
  uint16_t top_ = 0;
};

#include "FileBrowser.h"

#include "StorageManager.h"

void FileBrowser::begin(StorageManager& storage, const char* dir, const char* ext) {
  storage_ = &storage;
  dir_ = dir;
  ext_ = ext;
  refresh();
}

void FileBrowser::refresh() {
  count_ = storage_ ? storage_->listFiles(dir_, ext_, items_, MaxItems) : 0;
  selected_ = 0;
  top_ = 0;
}

void FileBrowser::move(int delta) {
  if (count_ == 0) return;
  int next = static_cast<int>(selected_) + delta;
  if (next < 0) next = 0;
  if (next >= count_) next = count_ - 1;
  selected_ = next;
  if (selected_ < top_) top_ = selected_;
  if (selected_ >= top_ + 10) top_ = selected_ - 9;
}

String FileBrowser::selectedName() const {
  return selected_ < count_ ? items_[selected_] : "";
}

String FileBrowser::selectedPath() const {
  if (selected_ >= count_) return "";
  String name = items_[selected_];
  if (name.startsWith("/")) return name;
  return String(dir_) + "/" + name;
}

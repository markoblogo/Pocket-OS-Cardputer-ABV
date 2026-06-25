#pragma once

#include <Arduino.h>

class StorageManager;

struct WifiEntry {
  String ssid;
  String pass;
};

class NetworkManager {
public:
  void begin(StorageManager& storage);
  void update();
  int scan();
  String scannedSsid(int index) const;
  int32_t scannedRssi(int index) const;
  bool scannedOpen(int index) const;
  bool connect(const String& ssid, const String& pass);
  bool reconnectKnown();
  bool connected() const;
  bool connecting() const { return connecting_; }
  bool failed() const { return failed_; }
  String ip() const;
  void saveKnown(const String& ssid, const String& pass);
  uint8_t knownCount() const { return knownCount_; }
  WifiEntry known(uint8_t index) const { return known_[index]; }
  int32_t rssi() const;

private:
  StorageManager* storage_ = nullptr;
  static constexpr uint8_t MaxKnown = 8;
  WifiEntry known_[MaxKnown];
  uint8_t knownCount_ = 0;
  int scanCount_ = 0;
  bool connecting_ = false;
  bool failed_ = false;
  uint32_t connectStarted_ = 0;
  void loadKnown();
};

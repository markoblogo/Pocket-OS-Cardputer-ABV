#include "NetworkManager.h"

#include "StorageManager.h"
#include <ArduinoJson.h>
#include <WiFi.h>

void NetworkManager::begin(StorageManager& storage) {
  storage_ = &storage;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(false);
  loadKnown();
}

void NetworkManager::update() {
  if (!connecting_) return;
  if (WiFi.status() == WL_CONNECTED) {
    connecting_ = false;
    failed_ = false;
  } else if (millis() - connectStarted_ > 15000) {
    connecting_ = false;
    failed_ = true;
  }
}

int NetworkManager::scan() {
  scanCount_ = WiFi.scanNetworks(false, true);
  return scanCount_;
}

String NetworkManager::scannedSsid(int index) const {
  if (index < 0 || index >= scanCount_) return "";
  return WiFi.SSID(index);
}

int32_t NetworkManager::scannedRssi(int index) const {
  if (index < 0 || index >= scanCount_) return 0;
  return WiFi.RSSI(index);
}

bool NetworkManager::scannedOpen(int index) const {
  if (index < 0 || index >= scanCount_) return false;
  return WiFi.encryptionType(index) == WIFI_AUTH_OPEN;
}

bool NetworkManager::connect(const String& ssid, const String& pass) {
  WiFi.begin(ssid.c_str(), pass.c_str());
  saveKnown(ssid, pass);
  connecting_ = true;
  failed_ = false;
  connectStarted_ = millis();
  return WiFi.status() == WL_CONNECTED;
}

bool NetworkManager::reconnectKnown() {
  for (uint8_t i = 0; i < knownCount_; ++i) {
    if (connect(known_[i].ssid, known_[i].pass)) return true;
  }
  return false;
}

bool NetworkManager::connected() const {
  return WiFi.status() == WL_CONNECTED;
}

String NetworkManager::ip() const {
  return connected() ? WiFi.localIP().toString() : "not connected";
}

int32_t NetworkManager::rssi() const {
  return connected() ? WiFi.RSSI() : 0;
}

void NetworkManager::saveKnown(const String& ssid, const String& pass) {
  for (uint8_t i = 0; i < knownCount_; ++i) {
    if (known_[i].ssid == ssid) {
      known_[i].pass = pass;
      return;
    }
  }
  if (knownCount_ < MaxKnown) known_[knownCount_++] = WifiEntry{ssid, pass};
  if (!storage_ || !storage_->ready()) return;
  JsonDocument doc;
  JsonArray arr = doc["networks"].to<JsonArray>();
  for (uint8_t i = 0; i < knownCount_; ++i) {
    JsonObject item = arr.add<JsonObject>();
    item["ssid"] = known_[i].ssid;
    item["pass"] = known_[i].pass;
  }
  String out;
  serializeJsonPretty(doc, out);
  storage_->writeText("/config/wifi.json", out);
}

void NetworkManager::loadKnown() {
  if (!storage_ || !storage_->ready() || !storage_->exists("/config/wifi.json")) return;
  JsonDocument doc;
  if (deserializeJson(doc, storage_->readText("/config/wifi.json"))) return;
  for (JsonObject item : doc["networks"].as<JsonArray>()) {
    if (knownCount_ >= MaxKnown) break;
    known_[knownCount_++] = WifiEntry{String(item["ssid"] | ""), String(item["pass"] | "")};
  }
}

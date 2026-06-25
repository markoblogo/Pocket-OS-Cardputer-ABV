#include "SettingsManager.h"

#include "StorageManager.h"
#include <ArduinoJson.h>

bool SettingsManager::begin(StorageManager& storage) {
  storage_ = &storage;
  if (!storage.ready()) return false;
  if (!storage.exists("/config/settings.json")) {
    save();
    return true;
  }
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, storage.readText("/config/settings.json"));
  if (err) return false;
  settings_.screenTimeoutSec = doc["screenTimeoutSec"] | settings_.screenTimeoutSec;
  settings_.brightness = doc["brightness"] | settings_.brightness;
  settings_.volume = doc["volume"] | settings_.volume;
  settings_.shuffle = doc["shuffle"] | settings_.shuffle;
  settings_.timezoneOffsetMin = doc["timezoneOffsetMin"] | settings_.timezoneOffsetMin;
  settings_.webPin = String(doc["webPin"] | "");
  return true;
}

void SettingsManager::save() {
  if (!storage_ || !storage_->ready()) return;
  JsonDocument doc;
  doc["screenTimeoutSec"] = settings_.screenTimeoutSec;
  doc["brightness"] = settings_.brightness;
  doc["volume"] = settings_.volume;
  doc["shuffle"] = settings_.shuffle;
  doc["timezoneOffsetMin"] = settings_.timezoneOffsetMin;
  doc["webPin"] = settings_.webPin;
  String out;
  serializeJsonPretty(doc, out);
  storage_->writeText("/config/settings.json", out);
}

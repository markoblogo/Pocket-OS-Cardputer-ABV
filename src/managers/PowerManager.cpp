#include "PowerManager.h"

#include "InputManager.h"
#include "SettingsManager.h"
#include <M5Cardputer.h>

void PowerManager::begin(InputManager& input, SettingsManager& settings) {
  input_ = &input;
  settings_ = &settings;
  lastActivity_ = millis();
  wakeDisplay();
}

void PowerManager::update(bool backgroundBusy) {
  if (!awake_ || keepAwake_ || backgroundBusy) return;
  uint32_t timeout = static_cast<uint32_t>(settings_->get().screenTimeoutSec) * 1000UL;
  if (timeout > 0 && millis() - lastActivity_ > timeout) screenOff();
}

void PowerManager::notifyUserActivity() {
  lastActivity_ = millis();
  if (!awake_) wakeDisplay();
}

void PowerManager::wakeDisplay() {
  awake_ = true;
  if (input_) input_->setDisplayAwake(true);
  M5Cardputer.Display.setBrightness(settings_ ? settings_->get().brightness : 160);
}

void PowerManager::screenOff() {
  awake_ = false;
  if (input_) input_->setDisplayAwake(false);
  M5Cardputer.Display.setBrightness(0);
}

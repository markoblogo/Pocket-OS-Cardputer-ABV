#pragma once

#include <Arduino.h>

class InputManager;
class SettingsManager;

class PowerManager {
public:
  void begin(InputManager& input, SettingsManager& settings);
  void update(bool backgroundBusy);
  void notifyUserActivity();
  void requestKeepAwake(bool keepAwake) { keepAwake_ = keepAwake; }
  bool displayAwake() const { return awake_; }
  void wakeDisplay();
  void screenOff();

private:
  InputManager* input_ = nullptr;
  SettingsManager* settings_ = nullptr;
  bool awake_ = true;
  bool keepAwake_ = false;
  uint32_t lastActivity_ = 0;
};

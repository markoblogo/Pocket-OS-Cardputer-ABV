#pragma once

#include "InputManager.h"

class TerminalUI;
class StorageManager;
class SettingsManager;
class PowerManager;
class NetworkManager;

struct AppContext {
  TerminalUI* ui = nullptr;
  StorageManager* storage = nullptr;
  SettingsManager* settings = nullptr;
  PowerManager* power = nullptr;
  NetworkManager* network = nullptr;
  InputManager* input = nullptr;
};

class App {
public:
  virtual ~App() = default;
  virtual void begin(AppContext& context) { ctx_ = &context; }
  virtual void update() {}
  virtual void draw() = 0;
  virtual void onInput(const InputEvent& event) = 0;
  virtual const char* getTitle() const = 0;
  virtual const char* getHelpLine() const = 0;
  virtual bool wantsBackgroundWork() const { return false; }
  virtual InputContext inputContext() const { return InputContext::Navigation; }

protected:
  AppContext* ctx_ = nullptr;
};

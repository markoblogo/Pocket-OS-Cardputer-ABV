#pragma once

#include "App.h"

class AppManager {
public:
  void begin(AppContext& context);
  void add(App* app);
  void update();
  void draw();
  void onInput(const InputEvent& event);
  bool backgroundBusy() const;
  App* current() const;

private:
  static constexpr uint8_t MaxApps = 16;
  AppContext* ctx_ = nullptr;
  App* apps_[MaxApps];
  uint8_t count_ = 0;
  int8_t selected_ = 0;
  int8_t active_ = -1;
  bool dirty_ = true;
  void drawMenu();
};

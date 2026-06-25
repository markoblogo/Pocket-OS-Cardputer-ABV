#include "AppManager.h"

#include "TerminalUI.h"
#include <cstring>

void AppManager::begin(AppContext& context) {
  ctx_ = &context;
}

void AppManager::add(App* app) {
  if (count_ >= MaxApps) return;
  apps_[count_++] = app;
  app->begin(*ctx_);
}

void AppManager::update() {
  if (active_ >= 0) apps_[active_]->update();
}

void AppManager::draw() {
  if (active_ >= 0) {
    apps_[active_]->draw();
    ctx_->ui->footer(apps_[active_]->getHelpLine());
  } else {
    drawMenu();
  }
}

void AppManager::onInput(const InputEvent& event) {
  if (event.action == InputAction::Wake) {
    if (active_ >= 0 && std::strcmp(apps_[active_]->getTitle(), "Input Test") == 0) {
      apps_[active_]->onInput(event);
    }
    return;
  }
  if (active_ >= 0) {
    if (event.action == InputAction::Back && event.type == InputEventType::LongPress) {
      active_ = -1;
      dirty_ = true;
      return;
    }
    apps_[active_]->onInput(event);
    return;
  }
  if (event.type != InputEventType::Press && event.type != InputEventType::Repeat) return;
  if (event.action == InputAction::Up && selected_ > 0) --selected_;
  else if (event.action == InputAction::Down && selected_ < static_cast<int8_t>(count_) - 1) ++selected_;
  else if (event.action == InputAction::Select || event.action == InputAction::Enter) active_ = selected_;
  dirty_ = true;
}

bool AppManager::backgroundBusy() const {
  for (uint8_t i = 0; i < count_; ++i) {
    if (apps_[i]->wantsBackgroundWork()) return true;
  }
  return false;
}

App* AppManager::current() const {
  return active_ >= 0 ? apps_[active_] : nullptr;
}

void AppManager::drawMenu() {
  ctx_->ui->header("Cardputer ADV Shell");
  for (uint8_t i = 0; i < count_ && i < 12; ++i) {
    ctx_->ui->listItem(i + 2, apps_[i]->getTitle(), i == selected_);
  }
  ctx_->ui->footer("GO:OPEN  \x18\x19:MOVE  HOLD GO:MENU");
}

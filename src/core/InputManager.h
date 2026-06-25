#pragma once

#include <Arduino.h>

enum class InputAction : uint8_t {
  None,
  Up,
  Down,
  Left,
  Right,
  Select,
  Back,
  Enter,
  TextChar,
  Backspace,
  Help,
  Wake
};

enum class InputEventType : uint8_t {
  Press,
  Release,
  Repeat,
  LongPress
};

struct InputEvent {
  InputAction action = InputAction::None;
  InputEventType type = InputEventType::Press;
  char text = '\0';
  uint32_t timestamp = 0;
};

struct InputTiming {
  uint16_t debounceMs = 30;
  uint16_t longPressMs = 700;
  uint16_t repeatStartMs = 400;
  uint16_t repeatEveryMs = 120;
};

class InputManager {
public:
  void begin(const InputTiming& timing = InputTiming{});
  void update();

  bool pollEvent(InputEvent& event);
  bool hasEvent() const;
  void clear();

  void setDisplayAwake(bool awake);
  bool isDisplayAwake() const;

private:
  static constexpr uint8_t kQueueSize = 24;
  static constexpr uint8_t kStateCount = 9;

  enum class PhysicalSource : uint8_t {
    BtnGo,
    KeyUp,
    KeyDown,
    KeyLeft,
    KeyRight,
    KeyEnter,
    KeyBackspace,
    KeyHelp,
    Count
  };

  struct ButtonState {
    bool stableDown = false;
    bool lastRawDown = false;
    bool longSent = false;
    bool suppressRelease = false;
    uint32_t rawChangedAt = 0;
    uint32_t pressedAt = 0;
    uint32_t nextRepeatAt = 0;
  };

  InputTiming timing_;
  bool displayAwake_ = true;
  ButtonState states_[kStateCount];

  InputEvent queue_[kQueueSize];
  uint8_t head_ = 0;
  uint8_t tail_ = 0;
  uint8_t count_ = 0;

  void scanDigitalSource(PhysicalSource source, bool rawDown, InputAction action, uint32_t now, bool repeatable);
  void emitPressOrWake(PhysicalSource source, InputAction action, uint32_t now);
  void push(InputAction action, InputEventType type, uint32_t now, char text = '\0');
  bool queueTextChar(char c, uint32_t now);
};

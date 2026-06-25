#include <Arduino.h>
#include <M5Cardputer.h>
#include <SD.h>
#include <SPI.h>

#include "AppManager.h"
#include "Apps.h"
#include "Features.h"
#include "InputManager.h"
#include "NetworkManager.h"
#include "PowerManager.h"
#include "SettingsManager.h"
#include "StorageManager.h"
#include "TerminalUI.h"

InputManager input;
TerminalUI ui;
StorageManager storage;
SettingsManager settings;
PowerManager power;
NetworkManager network;
AppManager apps;

AppContext context;

#if FEATURE_ULTRA_SAFE_BOOT
namespace {
constexpr uint8_t kSdSck = 40;
constexpr uint8_t kSdMiso = 39;
constexpr uint8_t kSdMosi = 14;
constexpr uint8_t kSdCs = 12;
constexpr uint32_t kSdSpeeds[] = {400000UL, 1000000UL, 4000000UL, 10000000UL, 25000000UL};
constexpr uint32_t kBootHeartbeatMs = 2000;
constexpr uint32_t kBtnLongPressMs = 700;

uint32_t g_lastHeartbeatMs = 0;
bool g_btnDown = false;
uint32_t g_btnDownAtMs = 0;
bool g_btnLongHandled = false;

void drawSafeBootScreen(const String& line5, const String& line6) {
  M5Cardputer.Display.fillScreen(0x0000);
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(0xFFFF, 0x0000);
  M5Cardputer.Display.setCursor(8, 8);
  M5Cardputer.Display.println("CARDPUTER ABVx");
  M5Cardputer.Display.println("v0.1.3b SAFE BOOT");
  M5Cardputer.Display.println("Serial: OK");
  M5Cardputer.Display.println("Display: OK");
  M5Cardputer.Display.println(line5.c_str());
  M5Cardputer.Display.setTextColor(0xFFE0, 0x0000);
  M5Cardputer.Display.println(line6.c_str());
  M5Cardputer.Display.println("1: input test");
  M5Cardputer.Display.println("2: launcher later");
}

void testSdFromBoot() {
  Serial.println("[BOOT] GO pressed, testing SD");
  Serial.flush();
  bool ok = false;
  String status = "SD: not tested";

  drawSafeBootScreen("SD: testing...", "GO: test SD");
  M5Cardputer.Display.setCursor(8, 72);
  M5Cardputer.Display.setTextColor(0x07FF, 0x0000);
  M5Cardputer.Display.println("testing... hold still");

  for (uint8_t i = 0; i < (sizeof(kSdSpeeds) / sizeof(kSdSpeeds[0])); ++i) {
    const uint32_t speed = kSdSpeeds[i];
    Serial.printf("[BOOT] SD test speed=%lu\n", speed);
    Serial.flush();

    SD.end();
    SPI.end();
    SPI.begin(kSdSck, kSdMiso, kSdMosi, kSdCs);

    const bool beginOk = SD.begin(kSdCs, SPI, speed);
    if (!beginOk) {
      Serial.printf("[BOOT] SD test fail at speed=%lu\n", speed);
      Serial.flush();
      continue;
    }

    const uint8_t cardType = SD.cardType();
    const uint64_t cardSize = SD.cardSize();
    if (cardType == CARD_NONE || cardType == CARD_UNKNOWN || !cardSize) {
      Serial.printf("[BOOT] SD present but invalid card type=%u size=%llu\n", cardType, cardSize);
      Serial.flush();
      continue;
    }

    ok = true;
    status = String("SD OK speed=") + speed + " card=" + String(cardType);
    Serial.printf("[BOOT] SD OK speed=%lu card=%u sizeMB=%llu\n", speed, cardType, cardSize / (1024ULL * 1024ULL));
    Serial.flush();
    break;
  }

  if (!ok) {
    status = "SD: failed";
  }
  drawSafeBootScreen(status, "GO: test SD");
}
}  // namespace
#endif

#if FEATURE_MUSIC
MusicApp musicApp;
#endif
#if FEATURE_RECORDER
RecorderApp recorderApp;
#endif
#if FEATURE_NOTES
NotesApp notesApp;
#endif
#if FEATURE_READER
ReaderApp readerApp;
#endif
#if FEATURE_CLOCK
ClockApp clockApp;
#endif
#if FEATURE_NETWORK
NetworkApp networkApp;
#endif
#if FEATURE_WEB_FILE_MANAGER
WebFileManagerApp webFileManagerApp;
#endif
#if FEATURE_RANDOMIZER
RandomizerApp randomizerApp;
#endif
#if FEATURE_BROWSER
BrowserApp browserApp;
#endif
#if FEATURE_AI_TEXT
AIApp aiApp;
#endif
#if FEATURE_PAYMENTS_INFO
PaymentsApp paymentsApp;
#endif
#if FEATURE_INPUT_DIAGNOSTICS
InputDiagnosticsApp inputDiagnosticsApp;
#endif
#if FEATURE_SYSTEM_INFO
SystemInfoApp systemInfoApp;
#endif

static void registerApps() {
  apps.begin(context);
#if FEATURE_MUSIC
  apps.add(&musicApp);
#endif
#if FEATURE_RECORDER
  apps.add(&recorderApp);
#endif
#if FEATURE_NOTES
  apps.add(&notesApp);
#endif
#if FEATURE_READER
  apps.add(&readerApp);
#endif
#if FEATURE_CLOCK
  apps.add(&clockApp);
#endif
#if FEATURE_NETWORK
  apps.add(&networkApp);
#endif
#if FEATURE_WEB_FILE_MANAGER
  apps.add(&webFileManagerApp);
#endif
#if FEATURE_RANDOMIZER
  apps.add(&randomizerApp);
#endif
#if FEATURE_BROWSER
  apps.add(&browserApp);
#endif
#if FEATURE_AI_TEXT
  apps.add(&aiApp);
#endif
#if FEATURE_PAYMENTS_INFO
  apps.add(&paymentsApp);
#endif
#if FEATURE_INPUT_DIAGNOSTICS
  apps.add(&inputDiagnosticsApp);
#endif
#if FEATURE_SYSTEM_INFO
  apps.add(&systemInfoApp);
#endif
}

void setup() {
#if FEATURE_ULTRA_SAFE_BOOT
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("[BOOT] 000 setup entered v0.1.3b ultra-safe");
  Serial.flush();
  Serial.println("[BOOT] 010 before M5Cardputer.begin");
  Serial.flush();

  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  Serial.println("[BOOT] 020 M5Cardputer.begin done");
  Serial.flush();

  drawSafeBootScreen("SD: not tested", "GO: test SD");
  Serial.println("[BOOT] 030 safe boot screen rendered");
  Serial.flush();

  g_lastHeartbeatMs = millis();
  g_btnDown = M5Cardputer.BtnA.isPressed();
  g_btnDownAtMs = g_btnDown ? g_lastHeartbeatMs : 0;
  g_btnLongHandled = false;
#else
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  Serial.begin(115200);

  ui.begin();
  ui.header("Boot");
  ui.line(2, "Init...");
  ui.line(4, "SD and modules");
  ui.pushFrame();

  input.begin();
  input.setDisplayAwake(true);
  const bool cardMounted = storage.begin();
  settings.begin(storage);
  network.begin(storage);
  power.begin(input, settings);
  context.ui = &ui;
  context.storage = &storage;
  context.settings = &settings;
  context.power = &power;
  context.network = &network;
  context.input = &input;
  registerApps();

  if (!cardMounted) {
    ui.clearFrame();
    ui.header("Storage");
    ui.line(2, "Insert SD card", TerminalUI::Yellow);
    ui.line(3, "GO retry / long GO menu", TerminalUI::Yellow);
    ui.pushFrame();
  }
#endif
}

void loop() {
#if FEATURE_ULTRA_SAFE_BOOT
  M5Cardputer.update();
  M5Cardputer.Keyboard.isChange();
  const uint32_t now = millis();

  if (now - g_lastHeartbeatMs >= kBootHeartbeatMs) {
    Serial.printf("[BOOT] loop alive ms=%lu heap=%u\n", now, ESP.getFreeHeap());
    Serial.flush();
    g_lastHeartbeatMs = now;
  }

  const bool btnDown = M5Cardputer.BtnA.isPressed();
  if (btnDown != g_btnDown) {
    g_btnDown = btnDown;
    g_btnDownAtMs = now;
    g_btnLongHandled = false;

    if (!btnDown) {
      if (!g_btnLongHandled) {
        testSdFromBoot();
      }
    }
  } else if (btnDown && !g_btnLongHandled && (now - g_btnDownAtMs >= kBtnLongPressMs)) {
    g_btnLongHandled = true;
    Serial.println("[BOOT] GO long press detected (diagnostic)");
    Serial.flush();
  }

  delay(10);
#else
  input.update();
  network.update();

  InputEvent event;
  while (input.pollEvent(event)) {
    if (event.action == InputAction::Wake) {
      Serial.println("[Power] wake requested");
      power.wakeDisplay();
      apps.onInput(event);
      continue;
    }
    power.notifyUserActivity();
    apps.onInput(event);
  }

  apps.update();
  power.update(apps.backgroundBusy());

  if (power.displayAwake()) apps.draw();
  delay(5);
#endif
}

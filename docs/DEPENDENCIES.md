# Dependency inventory

Status: maintained reference for the current source tree.

| Dependency | Current version | Source | Distribution boundary |
| --- | --- | --- | --- |
| ESP-IDF | 5.4.4 | https://github.com/espressif/esp-idf | Build toolchain; not vendored |
| M5Unified | 0.2.10 | https://github.com/m5stack/M5Unified | Vendored under `components/M5Unified`; preserve its license |
| M5GFX | 0.2.15 | https://github.com/m5stack/M5GFX | Vendored under `components/M5GFX`; preserve its license |
| minimp3 | vendored snapshot | https://github.com/lieff/minimp3 | Vendored under `components/minimp3`; preserve notices |
| Adafruit TCA8418 | local ESP-IDF port | https://github.com/adafruit/Adafruit_TCA8418 | Source under `main/lib`; preserve notices |
| parakeet-mlx/model weights | optional, user-installed | documented in `VOICE_TO_TEXT.md` | Never bundled in firmware or Companion |

## Update procedure

1. Update one firmware dependency per pull request and record its exact upstream
   tag or commit here.
2. Preserve upstream license files and document local patches.
3. Run host checks and a clean ESP-IDF build.
4. Run the hardware checklist for display, keyboard, SD, audio, Transfer and any
   dependency-specific surface. A successful build is not hardware acceptance.

Moving vendored M5 libraries to ESP-IDF Component Manager is deferred until the
same versions pass the physical-device matrix.

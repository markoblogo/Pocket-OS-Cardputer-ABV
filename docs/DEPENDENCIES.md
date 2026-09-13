# Dependency inventory

Status: maintained reference for the current source tree.

| Dependency | Current version | Source | Distribution boundary |
| --- | --- | --- | --- |
| ESP-IDF | 5.4.4 | https://github.com/espressif/esp-idf | Build toolchain; not vendored |
| M5Unified | 0.2.21 (`3eaaf828`) | https://github.com/m5stack/M5Unified | Vendored under `components/M5Unified`; preserve its license |
| M5GFX | 0.2.28 (`d91077b9`) | https://github.com/m5stack/M5GFX | Vendored under `components/M5GFX`; preserve its license |
| minimp3 | vendored snapshot | https://github.com/lieff/minimp3 | Vendored under `components/minimp3`; preserve notices |
| Adafruit TCA8418 | local ESP-IDF port | https://github.com/adafruit/Adafruit_TCA8418 | Source under `main/lib`; preserve notices |
| parakeet-mlx/model weights | optional, user-installed | documented in `VOICE_TO_TEXT.md` | Never bundled in firmware or Companion |

Local vendor patch: the M5Unified 0.2.21 `idf_component.yml` registry dependency
on M5GFX is removed because the matching M5GFX 0.2.28 source is already vendored
as a local component. This prevents ESP-IDF from downloading and compiling a
second copy; library source is otherwise taken from upstream tag 0.2.21.

Additional M5Unified 0.2.21 corrections define and bound the public LED type
accessor, reject out-of-range LED writes, accept time-only PowerHub alarms, and
clear the PowerHub alarm-enable bit with the correct mask.

## Update procedure

1. Update one firmware dependency per pull request and record its exact upstream
   tag or commit here.
2. Preserve upstream license files and document local patches.
3. Run host checks and a clean ESP-IDF build.
4. Run the hardware checklist for display, keyboard, SD, audio, Transfer and any
   dependency-specific surface. A successful build is not hardware acceptance.

Moving vendored M5 libraries to ESP-IDF Component Manager is deferred until the
same versions pass the physical-device matrix.

Local M5GFX 0.2.28 corrections: extended named colors are encoded as RGB565;
CST226 touch reports are clamped to caller capacity and successful coordinate
reads send the controller synchronization acknowledgement. These paths are not
used by Cardputer ADV, but remain safe for consumers of the vendored library.

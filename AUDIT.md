# ABVx Firmware Audit

This document records the current engineering audit state for the minimal ABVx Cardputer firmware.

## Current baseline

Project: `/Volumes/Work/Work/cardputer-abvx-minimal`

Target hardware: M5Stack Cardputer ADV / ESP32-S3.

Build baseline:

```sh
. ~/esp/esp-idf-v5.4.2/export.sh
idf.py build
```

Latest verified firmware artifact:

```text
build/cardputer-abvx-minimal.bin
```

The firmware has been repeatedly tested on real Cardputer ADV hardware. Core apps currently work at MVP level: Music, Record, Reader, Notes, Files, Time, Habits, Randomizer, Settings, Connections, Agent quick actions.

## Hardware assumptions verified

- Display: 240 x 135 ST7789V2 class display.
- MCU: ESP32-S3, 8 MB flash target.
- Keyboard: Cardputer matrix keyboard through TCA8418-style raw events.
- Storage: microSD through SPI/SDSPI, CS on GPIO 12.
- Audio: M5Unified speaker/mic paths work on device.
- Speaker output: `M5.Speaker.playRaw(...)` is proven by Audio Lab history and current Music/Record playback.
- Mic input: current Record app writes 16-bit mono 16 kHz WAV.
- USB runtime serial is not relied on for transfer. Connections Wi-Fi AP is the current transfer path.

## Audit fixes already applied

### Upload/session safety

- Chunk upload now uses smaller chunks and retries in `tools/cardputer_upload.py`.
- Firmware enforces a chunk size cap.
- `/api/upload-abort` is restricted to an active upload session and matching path.
- `upload-chunk` and `upload-finish` also require an active matching session.
- Stopping Connections clears upload session state.

### Audio/heap stability

- MP3 frame decode buffer is reused instead of allocated per audio chunk.
- PCM chunk reserves capacity before decode.
- Dead MP3 sync helper code was removed.

### SD/write reliability

- `flushAndClose()` centralizes flush/close checks for important writes.
- Config writes report RAM fallback if flush/close fails.
- Notes remove a newly-created note if write/close fails.
- Habits state/log/config writes now check close paths more consistently.
- Recorder detects short/failed writes, shows `WRITE ERR`, and removes a failed WAV on stop.
- FATFS open-file limit was raised from 5 to 8.

### UI/lifecycle consistency

- Message routing is centralized through `showMessage(...)`.
- Message return targets no longer rely on stale per-screen flag state.

## Current risk register

### P0: Wi-Fi upload architecture

Current upload still writes SD data inside HTTP handlers. This works for small files and may work with conservative chunk upload, but it is the highest remaining runtime-risk area.

Safer future design:

- HTTP handler receives small metadata/chunk request.
- Main loop performs SD write outside the HTTP server context.
- `/api/status` reports queued/running/done/error.
- Upload tool polls status.

Do this if large MP3 upload still drops AP or returns connection reset.

### P1: Monolithic `main.cpp`

`main/main.cpp` is now a large single file. This was acceptable for rapid MVP development but slows safe changes.

Recommended extraction order:

1. `input.*` for key normalization.
2. `storage.*` for SD paths, safe writes, list helpers.
3. `audio.*` for MP3/PCM/recorder helpers.
4. `ui.*` for theme/drawing primitives.
5. App modules only after the helpers are stable.

Do not do a broad rewrite before the next hardware-tested release.

### P1: SD hot-remove/remount

`sd_ready` is cached after first successful mount. If SD is removed or electrically glitches, apps may keep assuming SD is available.

Future fix:

- Add a lightweight SD health probe before writes.
- On repeated write/read failure, mark SD unavailable and show a clear error.
- Add Settings -> SD remount action.

### P2: Time persistence

Clock, timer, and alarm are runtime only. Full power-off resets time. This is acceptable for now because reliable sync is postponed.

Future fix:

- Set time once through Connections/Mac sync.
- Consider storing last manually-set time only as a hint, not as reliable wall clock.

### P2: Text/font layer

English and Russian are usable. French accents and Ukrainian-specific glyph coverage are not finalized.

Future fix:

- Define a formal supported glyph set.
- Add a compact font/glyph table for Cyrillic plus selected Latin accents.
- Keep input separate from display: translit input can remain for RU notes.

### P2: File manager delete policy

Files supports delete confirmation for files. There is no trash/recycle and no folder delete.

Future fix:

- Keep folder delete disabled.
- Add a safer delete screen with file size/path and maybe a second confirmation for `/music` and `/rec`.

### P3: Automated verification

There are no host tests for critical pure logic.

Good first tests:

- path whitelist / 8.3 validation;
- hidden/macOS file filtering;
- transliteration cases;
- message return routing;
- habit log parsing;
- upload session state machine.

## Release-readiness checklist

Before tagging a public firmware release:

- Build succeeds from a clean clone.
- README smoke test passes on real hardware.
- Music plays at least one real MP3 from SD.
- Record can save and play a WAV.
- Reader opens English and Russian TXT.
- Notes can create/open LAT and RU translit notes.
- Files can browse and open known file types.
- Connections ping/list/download/write-test pass.
- Upload either passes for a real MP3 or is clearly marked experimental.
- Firmware version/about screen exists.
- GitHub Release includes `.bin`, flash command, known limitations, and SD layout.

## Current recommendation

Do not add major new product apps until the current MVP is tagged as a test release.

Best next engineering steps:

1. Hardware-test the recent audit fixes.
2. If Connections large upload still fails, implement queued SD write outside HTTP handlers.
3. Add an About/version screen.
4. Add `CHANGELOG.md` and tag the first test release.
5. Then resume feature work from the roadmap.

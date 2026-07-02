# CardputerABVx

Minimal offline-first firmware for M5Stack Cardputer ADV.

ABVx is a small monochrome pocket shell focused on practical offline utilities: MP3 playback, voice recording, text reading, notes, time tools, files, habits, and simple transfer diagnostics.

## Status

Current firmware is tested on real Cardputer ADV hardware.

Working blocks:

- ABVx boot splash.
- Large black/monochrome launcher with battery indicator.
- Music MP3 playback from SD with waveform.
- Voice recorder with WAV save/playback and waveform.
- TXT Reader with English/Russian display and speed-reading mode.
- Notes with plain text save/open and RU translit-save mode.
- Files SD browser with known file opening.
- Time: clock, stopwatch, timer, alarm.
- Habits checklist with manual day rollover and 7D/30D stats.
- Randomizer: `YES / NO / MB`.
- Settings: theme, sound, timeout, power-save preset, SD/config status.
- Connections: Wi-Fi AP read-only list/download MVP.

## Apps

### Launcher

- Up / Down selects apps.
- OK opens selected app.
- GO is a quick Music shortcut on launcher.
- Production launcher currently exposes user apps only; dev diagnostics are not in the normal build.

### Music

Reads MP3 files from `/sdcard/music`.

Controls:

- OK: play selected track.
- OK / GO while playing: stop and return to list.
- Up / Down: volume `MUTE / MID / LOUD`.
- Left / Right: previous / next track.
- `1`: shuffle toggle.

Notes:

- Waveform is drawn from decoded PCM chunks.
- Playback is chunk/blocking-based, so controls can have small latency.
- EOF auto-advances when multiple tracks exist.

### Record

Stores recordings in `/sdcard/rec` as 16-bit mono 16 kHz WAV.

Controls:

- `NEW REC` -> OK starts recording.
- OK / GO stops and saves.
- OK on a recording plays it.

### Reader

Reads `.TXT` files from `/sdcard/books`.

Features:

- English and Russian UTF-8 display are supported best-effort.
- Up / Down scroll lines.
- Left / Right page.
- `1` enters speed-reading mode.
- In-RAM bookmark returns to the last viewed line while the device remains on.

Speed-reading:

- Modes: `1W`, `2W`, `LINE`.
- Range: `350..1000 WPM`, step `50`.
- OK pause/resume.
- Up / Down speed.
- Left / Right mode.
- GO returns to normal reader.

### Notes

Stores notes in `/sdcard/notes` as `NOTE0001.TXT`, `NOTE0002.TXT`, etc.

Features:

- `NEW NOTE` creates a note.
- LAT mode saves typed latin text.
- RU mode saves transliterated Russian text.
- `1` toggles LAT/RU while editing.
- Saved notes open in the same text viewer as Reader.

RU translit is intentionally simple. Example: `privet eto anton` -> Russian text in the saved note. The edit screen still shows latin input.

### Files

Browse SD folders and open known file types.

Known openings:

- `.MP3` -> Music playback.
- `.TXT` -> Reader/Notes viewer depending on path.
- `.WAV` / `.PCM` -> Record playback path.

Files also shows SD free/used status. Delete/rename are postponed.

### Time

Modes:

- Clock.
- Stopwatch with tenths.
- Timer with hour/min/sec fields.
- Alarm with hour/min/sec fields and sound alert.

Controls:

- Left / Right switches mode.
- OK starts/stops/arms depending on mode.
- `1` changes setup field or resets where appropriate.
- Up / Down changes selected field.

Clock is runtime/manual only. It resets after full power-off; future Mac sync should set time during transfer/sync.

### Habits

Stores routines in `/sdcard/habits`.

Files:

- `/sdcard/habits/HABITS.TXT`
- `/sdcard/habits/LOG.TXT`
- `/sdcard/habits/STATE.TXT`

Features:

- Today checklist.
- OK toggles selected habit.
- `1` starts the next internal day and clears checks.
- Right opens 7D/30D stats.
- Left opens manage screen.
- Add habit.
- Disable selected habit without deleting old logs.

Calendar dates are postponed until reliable time sync exists.

### Randomizer

- OK rolls `YES`, `NO`, or `MB` using `esp_random()`.
- No history yet.

### Settings

Stored in `/sdcard/cardputer/CONFIG.TXT`.

Options:

- Theme: `WHITE`, `GREEN`, `YELLOW`, `INVERT`.
- Sound: `OFF`, `LOW`, `MID`, `LOUD`, `MAX`.
- Timeout: `SHORT`, `NORMAL`, `LONG`.
- Power preset: green theme, low sound, short timeout.
- SD/config status.
- Communications placeholder.

### Connections

Current MVP starts a Cardputer Wi-Fi AP and exposes read-only SD browsing endpoints.

AP:

- SSID: `ABVX-Cardputer`
- Password: `cardputer`
- URL: `http://192.168.4.1`

Endpoints:

- `/` simple status page.
- `/api/ping` returns `OK PING`.
- `/api/status` returns AP/HTTP/request status.
- `/api/list?path=/music` lists a whitelisted SD folder.
- `/api/download?path=/notes/NOTE0001.TXT` downloads a whitelisted SD file.
- `/api/write-test` writes `/sdcard/cardputer/WTEST.TXT` for SD write diagnostics; GET and POST are both accepted for easier browser testing.

Upload and delete are intentionally not implemented yet.

## SD card layout

Use FATFS 8.3-safe names for now.

```text
/sdcard/music/A.MP3
/sdcard/music/T01.MP3
/sdcard/books/EN1.TXT
/sdcard/books/RU1.TXT
/sdcard/notes/NOTE0001.TXT
/sdcard/rec/REC0001.WAV
/sdcard/habits/HABITS.TXT
/sdcard/cardputer/CONFIG.TXT
```

Long filenames are not treated as reliable yet.

## Build

```sh
cd /Volumes/Work/Work/cardputer-abvx-minimal
. ~/esp/esp-idf-v5.4.2/export.sh
idf.py build
```

Firmware output:

```text
build/cardputer-abvx-minimal.bin
```

## Flash

Find the current port:

```sh
ls /dev/cu.usbmodem*
```

Flash:

```sh
idf.py -p /dev/cu.usbmodem101 flash
```

Replace `/dev/cu.usbmodem101` with the actual port.

## Quick hardware smoke test

1. Boot and confirm ABVx splash.
2. Confirm launcher opens and Up / Down navigation works.
3. Open Music, play `A.MP3`, confirm sound and waveform, stop with OK/GO.
4. Open Record, create a short recording, save, play it back.
5. Open Reader, open a `.TXT`, scroll, enter speed mode with `1`, return with GO.
6. Open Notes, create and save a note, reopen it.
7. Open Time, test stopwatch and timer alert.
8. Open Files, browse SD and open a known `.TXT` or `.MP3`.
9. Open Habits, check an item, reopen, confirm state remains.
10. Open Settings, change theme, reboot if needed, confirm config loads.
11. Open Connections, press OK, connect Mac to `ABVX-Cardputer`, test `/api/ping`, `/api/list?path=/music`, one `/api/download?...`, and `/api/write-test`, stop with GO.

## Known limitations

- FATFS long filename support is not finalized; use 8.3-safe names. macOS AppleDouble sidecar aliases are hidden from SD lists.
- Reader/Notes text layer is English/Russian best-effort. French accents and Ukrainian-specific polish are postponed.
- Notes RU input is translit-save, not a native Cyrillic keyboard.
- Reader bookmarks are RAM-only for now.
- Music playback is stable enough for MVP but still uses chunked blocking playback.
- Files is read/open only; delete/rename need confirmation UX later.
- Connections supports list/download plus a small write-test endpoint. Full upload/delete are next steps.
- Clock does not persist through full power-off.
- Habits use manual day rollover, not calendar dates.
- Browser, AI, Mac companion sync, and full Agent are postponed.

## Roadmap

Near-term:

1. Connections v3: safe upload with 8.3 validation and queued SD writes after write-test is confirmed stable.
2. Local web file manager: browser UI for list/download/upload/status while Cardputer runs AP mode.
3. Files v2: size/details and unsupported-file screen polish.
4. Reader v2: persistent bookmarks.
5. Notes v2: edit existing notes.
6. Time sync through Connections or future Mac sync flow.
7. Agent quick-actions menu after app actions are stable.

Infrastructure later:

- Firmware version/about screen with build info, SD status, and battery.
- `CHANGELOG.md` and tagged GitHub releases.
- GitHub Actions build that publishes `.bin` firmware artifacts.
- Power policy hardening: Wi-Fi only while Connections is open, screen dim/off rules, no deep sleep until state handling is reliable.
- OTA updates only after release/versioning/rollback discipline is in place.

Longer-term product ideas:

- Text-only browser: fetch pages, remove images/video/scripts, keep readable text and links. This stays offline-shell-adjacent and is not a priority until core apps and transfer are stable.
- Mac companion app for sync, file preparation, time setting, config, and firmware updates.
- Offline Agent as quick-actions command center first; online AI later and optional.

Out of scope for this firmware:

- Agro dashboards, grain-market terminals, price alerts, WebSocket market feeds, subscriptions, and Meshtastic. These may belong to separate projects, not this offline personal shell.

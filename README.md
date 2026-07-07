# CardputerABVx

Minimal offline-first firmware for M5Stack Cardputer ADV.

ABVx is a monochrome pocket shell for practical utilities: MP3 playback, short voice notes, TXT reading, plain notes, time tools, SD files, habits, randomizer, settings, and Wi-Fi transfer diagnostics.

## Current status

Tested on real Cardputer ADV hardware.

Working:

- ABVx boot splash and large text launcher.
- Music: MP3 playback from SD with waveform, volume, next/prev, shuffle.
- Record: short WAV voice notes, save/playback, waveform.
- Reader: TXT books, English/Russian display, speed-reading, persistent bookmarks.
- Notes: LAT/plain text create/open/edit/delete. Cyrillic notes are view-only.
- Time: clock, stopwatch, timer, alarm.
- Files: SD browser, known file opening, file info/delete.
- Habits: daily checklist, manual next day, 7D/30D stats.
- Randomizer: `YES / NO / MB`.
- Settings: theme, sound, timeout, power preset, SD reprobe, About.
- Connections: Wi-Fi AP list/download/upload MVP.

Agent, browser, AI, Mac companion, and long-recording are postponed.

## Controls

Global:

- Up / Down: move selection.
- Left / Right: page or switch mode where supported.
- OK / Enter: open, start, save, confirm.
- GO / Back: return or stop.
- Backspace: delete where supported.
- `1`: app-specific shortcut.

App highlights:

- Music: OK play/stop, Up/Down volume, Left/Right track, `1` shuffle.
- Reader: Up/Down line, Left/Right page, `1` speed mode, OK pause in speed mode.
- Notes: `1` new/edit, Backspace delete from list. Notes editor is LAT/plain text only.
- Record: OK starts `NEW REC`, OK/GO stops and saves, OK plays saved recording.
- Habits: OK toggles, `1` next day, Right stats, Left manage.
- Connections: OK starts AP, GO stops it.

## SD layout

Use 8.3-safe filenames for now.

```text
/sdcard/music/A.MP3
/sdcard/books/EN1.TXT
/sdcard/books/RU1.TXT
/sdcard/notes/NOTE0001.TXT
/sdcard/rec/REC0001.WAV
/sdcard/RECS/REC0001.WAV
/sdcard/habits/HABITS.TXT
/sdcard/habits/LOG.TXT
/sdcard/habits/STATE.TXT
/sdcard/CARDPTR/CONFIG.TXT
/sdcard/CARDPTR/READER.TXT
```

## Connections MVP

Wi-Fi AP:

- SSID: `ABVX-Cardputer`
- Password: `cardputer`
- URL: `http://192.168.4.1`

Useful endpoints:

```sh
curl http://192.168.4.1/api/ping
curl http://192.168.4.1/api/status
curl "http://192.168.4.1/api/list?path=/music"
curl "http://192.168.4.1/api/download?path=/notes/NOTE0001.TXT"
curl http://192.168.4.1/api/write-test
```

Chunk upload tool:

```sh
python3 tools/cardputer_upload.py ./T01.MP3 /music/T01.MP3
```

Upload is still an MVP: 8.3 names, whitelisted folders, no overwrite, conservative chunks.

## Build

```sh
cd /Volumes/Work/Work/cardputer-abvx-minimal
. ~/esp/esp-idf-v5.4.2/export.sh
idf.py build
```

Firmware:

```text
build/cardputer-abvx-minimal.bin
```

Flash:

```sh
ls /dev/cu.usbmodem*
idf.py -p /dev/cu.usbmodem101 flash
```

## Smoke test

1. Boot: ABVx splash and launcher.
2. Music: play `A.MP3`, check sound/waveform, stop.
3. Record: create short note, confirm `Record saved`, play it back.
4. Reader: open TXT, scroll, speed mode, exit/reopen and confirm bookmark.
5. Notes: create LAT note, edit it, delete it; Cyrillic note should be view-only.
6. Time: stopwatch and timer/alarm sound.
7. Files: browse SD, open TXT/MP3/WAV, open unsupported file info.
8. Habits: toggle habit, next day, stats.
9. Settings: SD reprobe, About, config/theme.
10. Connections: AP starts, `/api/ping`, list, download, write-test.

## Known limitations

- FATFS long filenames are not reliable yet; use 8.3-safe names.
- Music uses chunked/blocking playback, so controls can have small latency.
- Recorder is short-note MVP; long recording needs streaming/ring-buffer work.
- Notes editor is LAT/plain text only; Cyrillic files are view-only.
- Reader Cyrillic display is custom/best-effort; French/Ukrainian polish is postponed.
- Clock resets after full power-off until Mac/time sync exists.
- Habits use manual day rollover, not calendar dates.
- Connections upload is useful but still the riskiest transfer path.

## Roadmap

Near-term:

1. Files v2 polish: details, unsupported files, safer delete, transfer folder.
2. Record v2: longer recording architecture, delete confirm, playback progress.
3. Connections v2: safer queued upload and phone-friendly web UI.
4. Time/Habits polish: presets, persistence, cleaner summaries.
5. Browser MVP: text-only pages, favorites, simple downloads.
6. Agent/AI: later, only when it adds value beyond launcher.
7. Mac companion: sync, file preparation, time setting, firmware updates.

Out of scope for this firmware: agro dashboards, market terminals, WebSocket price alerts, subscriptions, Meshtastic.

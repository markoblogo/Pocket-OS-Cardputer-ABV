# CardputerABVx

ABVx is a **Pocket OS** for M5Stack Cardputer ADV: a fast offline-first personal tool for capture, memory, reading/listening, routines, and transfer.

It is not just a launcher or a set of apps. The product goal is to minimize friction for common actions:

```text
Capture -> Remember -> Read -> Listen -> Act -> Reflect
```

The existing apps are implementation modules behind those actions.

## Product direction

Current firmware is the technical baseline for ABVx Pocket OS.

Core ideas:

- **One Button Capture**: start voice/text capture with one shortcut.
- **Universal Inbox**: notes, voice, habits, bookmarks, and actions flow into one log.
- **Timeline**: a simple life/activity journal built from captured events.
- **Context Resume**: resume last book, track, note, timer, or habit state.
- **Fast Dashboard**: boot into status + resume actions, not only an app list.
- **Zero Cursor Philosophy**: use physical keys directly where possible.
- **Progressive Apps**: simple first screen, advanced options later.
- **Minimal / Art mode**: productive default UI, optional battery-gated visual mode.

Architecture: [`docs/PRODUCT_ARCHITECTURE.md`](docs/PRODUCT_ARCHITECTURE.md)
Decisions: [`docs/ARCHITECTURE_DECISIONS.md`](docs/ARCHITECTURE_DECISIONS.md)
Roadmap: [`docs/ROADMAP.md`](docs/ROADMAP.md)

## Current status

Tested on real Cardputer ADV hardware.

Stable baseline:

- Boot: ABVx splash and large monochrome launcher.
- Music: SD MP3 player with waveform, volume, next/prev, shuffle, double-buffered playback.
- Record: WAV voice notes, RAM-first capture, save/play/delete, continuous playback, waveform.
- Reader: TXT books, English/Russian display, speed reading, persistent bookmarks.
- Notes: LAT/plain text create/open/edit/delete. Cyrillic notes are view-only.
- Time: manual clock, stopwatch, timer, alarm.
- Files: SD browser, file opening, file info, delete confirmation.
- Habits: daily checklist, manual next day, 7D/30D stats.
- Randomizer: `YES / NO / MB`.
- Settings: theme, sound, timeout, power preset, SD reprobe, About.
- Connections: Wi-Fi AP list/download/small-upload diagnostics.

Postponed: browser, AI, Mac companion, long SD-streaming recorder, large Wi-Fi upload.

Detailed status: [`docs/PROJECT_STATUS.md`](docs/PROJECT_STATUS.md)

## Controls

Global:

- Up / Down: move selection.
- Left / Right: page or switch mode.
- OK / Enter: open, start, save, confirm.
- GO / Back: return or stop.
- Backspace: delete where supported.
- `1`: app-specific shortcut.

App highlights:

- Music: OK play/stop, Up/Down volume, Left/Right track, `1` shuffle.
- Reader: Up/Down line, Left/Right page, `1` speed mode, OK pause/resume in speed mode.
- Notes: `1` new/edit, Backspace delete from list.
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
/sdcard/inbox/INBOX.TXT
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

Small upload only:

```sh
python3 tools/cardputer_upload.py ./NOTE.TXT /notes/NOTE.TXT
```

Upload is intentionally limited to 64KB. Large MP3 upload over Wi-Fi is unstable on Cardputer ADV and is disabled. Use an SD reader for music files.

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
2. Music: play `A.MP3`, check smooth sound/waveform, stop.
3. Record: create short note, confirm `Record saved`, play it back, delete test file.
4. Reader: open TXT, scroll, speed mode, exit/reopen and confirm bookmark.
5. Notes: create LAT note, edit it, delete it; Cyrillic note should be view-only.
6. Time: stopwatch, timer, alarm sound.
7. Files: browse SD, open TXT/MP3/WAV, open unsupported file info.
8. Habits: toggle habit, next day, stats.
9. Settings: SD reprobe, About, config/theme.
10. Connections: AP starts, `/api/ping`, list, download, write-test.

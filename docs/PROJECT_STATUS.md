# ABVx Project Status

Current baseline: offline-first Cardputer ADV firmware with stable local apps. Product direction is now **ABVx Pocket OS**: fast capture, memory, reading/listening, routines, and transfer.

## Working hardware-verified features

- Launcher: large monochrome list UI, app navigation, battery indicator.
- Music: MP3 files from `/sdcard/music`, double-buffered 16 kHz mono playback, waveform, volume, shuffle, prev/next.
- Record: WAV notes in `/sdcard/rec` or `/sdcard/RECS`, RAM-first capture, recording waveform, continuous playback, delete confirmation.
- Reader: `.TXT` books from `/sdcard/books`, normal reading, speed mode, persistent bookmark state.
- Notes: `.TXT` notes from `/sdcard/notes`, LAT/plain create/edit/delete, Cyrillic view-only.
- Files: SD browser, known file opening, unsupported file info, delete confirmation.
- Time: manual clock, stopwatch with fractions, timer, alarm sound.
- Habits: routines list, daily checks, manual next-day rollover, 7D/30D summaries.
- Randomizer: simple yes/no/maybe decision utility.
- Settings: theme, sound, timeout, power preset, SD reprobe, About.
- Transfer/Connections: Wi-Fi AP, ping/status/list/download/write-test, small upload limit.

## Product reframing

The apps are no longer the top-level product idea. They are implementation modules behind Pocket OS actions:

- Capture: Record, Notes, future One Button Capture.
- Remember: Notes, Habits, future Inbox/Timeline.
- Read: Reader, prepared Browser pages.
- Listen: Music, Record playback.
- Act: Time, Randomizer, Files.
- Reflect: Habits stats, future Timeline/Dashboard.

## Safety decisions

- Large Wi-Fi upload is disabled at 64KB. It caused SD I/O instability during MP3-sized transfers.
- Music upload should use SD reader/hub until the transfer stack is redesigned.
- FATFS long filename support is not treated as reliable yet; use 8.3 names.
- Agent is hidden/postponed until it becomes more than a duplicate launcher.
- Browser, AI, and Mac Companion are planned but must not destabilize offline apps.

## Known limitations

- Recorder is RAM-first MVP. It can use longer PSRAM buffers, but true SD streaming recorder is postponed.
- Clock resets after full power-off until time sync exists.
- Notes editor is LAT/plain text only; Cyrillic files are view-only.
- Reader Cyrillic is custom/best-effort; French/Ukrainian polish is postponed.
- Connections is useful for diagnostics/list/download/small files, not large media sync.

## Architecture

Architecture decisions are tracked in `docs/ARCHITECTURE_DECISIONS.md`.

Key decisions: Pocket OS action-first framing, AI online-only, no standalone Agent for now, Browser prepared-first, Mac Companion handles heavy conversion/sync, and large Wi-Fi upload remains disabled.

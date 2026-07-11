# ABVx Project Status

Current baseline: **post-v0.2 Pocket OS checkpoint**.

ABVx is an offline-first Cardputer ADV Pocket OS for fast capture, memory, reading/listening, routines, and transfer.

## Working hardware-verified features

- Launcher/Dashboard: large monochrome launcher, Resume, current time, routines progress, current context, and battery/low-voltage diagnostics.
- Music: MP3 files from `/sdcard/music`, smoother buffered 16 kHz mono playback, waveform, MAX volume, shuffle, prev/next, sorted library, track info/probe, and bad-track screen.
- Record: one hardware-verified 20-second 8 kHz/8-bit WAV mode, RAM-first capture, waveform, playback, delete confirmation.
- Reader: `.TXT` books from `/sdcard/books`, normal reading, speed mode, persistent bookmark state.
- Notes: `.TXT` notes from `/sdcard/notes`, LAT/plain create/edit/delete, Cyrillic view-only.
- Files: SD browser, `TRANSFER` folder for `/sdcard/CARDPTR`, known file opening, unsupported file info, delete confirmation.
- Time: manual clock, stopwatch with fractions, timer presets, alarm sound.
- Habits: larger routines list, daily checks, manual next-day rollover, rename, confirmed disable, restore disabled habits, streaks, and 7D/30D/365D summaries.
- Randomizer: simple yes/no/maybe decision utility.
- Settings: theme, sound, timeout, power preset, SD reprobe, About.
- Transfer/Connections: temporary Wi-Fi AP with per-session password, ping/status/list/download/write-test, and 64 KB small-file upload limit. It is not yet a full SD flash-drive replacement.

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

- Recorder is RAM-first because Cardputer ADV has no PSRAM and live SD writes during microphone capture are unsafe.
- Battery percentage is an estimate from Cardputer/M5 power readings; low or unstable samples may show diagnostic voltage instead of a false percentage.
- Clock resets after full power-off until time sync exists.
- Notes editor is LAT/plain text only; Cyrillic files are view-only.
- Reader Cyrillic is custom/best-effort; French/Ukrainian polish is postponed.
- Connections is useful for diagnostics/list/download/small files, not large media sync.
- Inbox is present as an early Pocket OS layer but must not do background SD writes during normal app use.

## Architecture

Architecture decisions are tracked in `docs/ARCHITECTURE_DECISIONS.md`.

Key decisions: Pocket OS action-first framing, AI online-only, no standalone Agent for now, Browser prepared-first, Mac Companion handles heavy conversion/sync, and large Wi-Fi upload remains disabled.

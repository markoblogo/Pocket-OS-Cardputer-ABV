# Changelog

## Unreleased checkpoint

- Listen: MAX volume label, shuffle fallback key, resilient restart after Settings, and heap preflight that reuses existing audio buffers.
- Listen: selected-track marquee, `2`/`I` track info screen, original upload title display, cached file size/status, safe `2`/`P` MP3 sync probe, and zero-byte filtering in the FatFS fallback.
- Time: timer presets for 1/5/10/20 minutes.
- Dashboard: current time, Resume, routines progress, SD status, direct Listen/Read/Write shortcuts, and battery/low-voltage diagnostics.
- Routines: streaming history persistence without the former 256-line truncation, 7D/30D/365D stats, current streaks, larger list text, rename, confirmed disable, keyboard fallbacks for manage/stats, and restore-all for disabled habits.
- Files/Transfer: `/sdcard/CARDPTR` is shown as `TRANSFER`, unsupported files use `FILE INFO`, and Connections states list/download/small-upload limits on device and web root.
- Dashboard: shows current Resume target plus last music, book, note, and recording context with clearer shortcut footer.
- Settings/About: compact SD/BAT/Transfer password status and clearer build/About screen.

## v0.2.0 - Release Baseline

Hardware-tested baseline for ABVx Pocket OS on Cardputer ADV.

### Stable

- Large monochrome launcher with one-button shortcuts.
- Listen: MP3 playback from SD with waveform, shuffle, volume, and smoother buffered playback.
- Voice: 20-second RAM-first WAV voice notes with save, playback, and delete.
- Read: small and large TXT books, English/Russian display, speed mode, and persistent bookmarks.
- Write: plain LAT notes with create, open, edit, and delete. Cyrillic notes are view-only.
- Time: manual clock, stopwatch, timer, and alarm.
- Files: SD browser, file info, known file opening, and delete confirmation.
- Routines: daily checklist and 7D/30D stats.
- Settings: theme, sound, timeout, power preset, SD reprobe, and About.
- Transfer: Wi-Fi AP list/download/small-file diagnostics.

### Important Limits

- Large Wi-Fi upload remains disabled; use SD reader/hub for MP3 libraries.
- FATFS long filenames are not a reliability target yet; 8.3-safe names are preferred.
- Voice recording is intentionally limited to 20 seconds.
- Agent, Browser, AI, Mac Companion, Bluetooth, GPS, and LoRa are not part of this baseline.

## Earlier

- Minimal ESP-IDF Cardputer ADV firmware split from the M5 UserDemo/Mooncake app framework.
- ABVx splash, Pocket OS launcher, Music MVP, Reader, Notes, Record, Time, Files, Habits, Settings, and Connections were built iteratively from hardware tests.

# Changelog

## Unreleased

- Added Connections v3 staged upload with bounded chunks, main-loop SD ownership, temporary-file verification, abort, timeout, and CLI recovery.
- Added non-repeating Music shuffle cycles and FAT-safe library preparation for Cyrillic, Hebrew, and other Unicode filenames.
- Enabled heap-backed FATFS long filenames and separated Music display names from playback paths: Cyrillic/Hebrew names are shown in full while decoding uses stable FAT short aliases.
- Filtered 4 KB macOS AppleDouble MP3 sidecars exposed as non-hidden FAT aliases, preventing false `BAD MP3 / no mpeg sync` entries.
- Added Unicode-safe Music marquee plus compact Cyrillic and Hebrew title glyphs with basic Hebrew RTL run handling.
- Added per-track FAT alias-to-LFN fallback resolution, fixing Unicode files whose short alias is rejected by POSIX `stat`.
- Removed POSIX `stat` as a playback gate; Probe and Playback now open alias/LFN candidates directly and determine size from the opened stream.
- Replaced Music's POSIX `FILE*` input stream with direct FatFS `FIL` open/read/seek/size operations to support aliases rejected by the VFS adapter.
- Replaced the technical FatFS `FR_INVALID_NAME` error with the user-facing `Unsupported filename`; malformed directory entries are skipped without destabilizing playback.

## Unreleased

- Persistence: introduced a tested POSIX event-log module with bounded 64-event history and temp-file replacement.
- Inbox/Timeline: moved persistence from RAM-only state to internal SPIFFS; queued events are committed only from the main loop on Launcher/Inbox screens.
- Voice: records and plays from internal SPIFFS and queues Timeline events without touching SD after microphone capture.
- SD: removable storage remains dedicated to user media, books, and external notes; Inbox/Voice writes no longer share its lifecycle.

## v0.2.2-test - Pocket OS Polish Checkpoint

- Listen: MAX volume label, shuffle fallback key, resilient restart after Settings, and heap preflight that reuses existing audio buffers.
- SD: manual reprobe now clears stale card state; Read/Write lists use read-only FatFS fallback like Listen, avoiding false empty Books/Notes after VFS instability.
- SD: Voice list and Files browser now also use read-only FatFS fallback; Voice list no longer creates recording dirs while browsing.
- Voice/Files: Files, Notes, and Voice lists prefer FatFS directory reads; Voice saves WAV via FatFS write path instead of POSIX `fopen/fwrite`.
- Files/Voice: Files root is now a virtual known-folder menu; Voice saves to an existing recording folder or `/CARDPTR` without attempting unsafe directory creation.
- Voice: if recording folders are unavailable, WAV files can fall back to existing `/notes` so capture still works without unsafe mkdir.
- Listen: selected-track marquee, `2`/`I` track info screen, original upload title display, cached file size/status, safe `2`/`P` MP3 sync probe, and zero-byte filtering in the FatFS fallback.
- Time: timer presets for 1/5/10/20 minutes.
- Dashboard: current time, Resume, routines progress, SD status, direct Listen/Read/Write shortcuts, and battery/low-voltage diagnostics.
- Routines: streaming history persistence without the former 256-line truncation, 7D/30D/365D stats, current streaks, larger list text, rename, confirmed disable, keyboard fallbacks for manage/stats, and restore-all for disabled habits.
- Files/Transfer: `/sdcard/CARDPTR` is shown as `TRANSFER`, unsupported files use `FILE INFO`, and Connections states list/download/small-upload limits on device and web root.
- Dashboard: shows current Resume target plus last music, book, note, and recording context with clearer shortcut footer.
- Settings/About: compact SD/BAT/Transfer password status and clearer build/About screen.
- Inbox/Timeline: safe RAM-only session event viewer with detail screen and explicit `1 REFRESH`; SD persistence is disabled after hardware SD instability.

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
- Transfer writes still prefer 8.3-safe names; Music reads now support UTF-8 long filenames.
- Voice recording is intentionally limited to 20 seconds.
- Agent, Browser, AI, Mac Companion, Bluetooth, GPS, and LoRa are not part of this baseline.

## Earlier

- Minimal ESP-IDF Cardputer ADV firmware split from the M5 UserDemo/Mooncake app framework.
- ABVx splash, Pocket OS launcher, Music MVP, Reader, Notes, Record, Time, Files, Habits, Settings, and Connections were built iteratively from hardware tests.

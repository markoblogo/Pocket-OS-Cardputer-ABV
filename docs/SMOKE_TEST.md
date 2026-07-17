# ABVx Smoke Test

Use this checklist after flashing a release checkpoint.

## Boot and Navigation

1. Boot shows the ABVx splash.
2. Launcher opens with large menu text.
3. Up/Down moves selection.
4. OK opens the selected module.
5. GO returns to launcher.

## Listen / Music

1. Open `LISTEN`.
2. MP3 list appears quickly.
3. OK plays a track with waveform.
4. Up/Down changes volume.
5. Left/Right changes track.
6. `1` or `S` toggles shuffle.
7. In shuffle mode, play/skip through the full library and confirm no track repeats before the cycle completes.
8. Cyrillic and Hebrew physical filenames open and play directly; `prepare_music.py` remains an optional ASCII-normalization tool.
9. macOS `._*.MP3` sidecars and their 4 KB FAT aliases do not appear in the Music list.
10. Cyrillic/Hebrew titles render as glyphs rather than squares in Music List, Track Info, and Listening; long titles marquee without broken UTF-8 characters.
11. A malformed FAT name shows `Unsupported filename`; Music remains responsive and shuffle continues with the next playable track.
12. In Music List, Up on the first track wraps to the last track and Down wraps back to the first.
13. In Launcher, Up on the first app wraps to the last app and Down wraps back to the first.
7. `2` or `I` opens `TRACK INFO`.
8. `2` or `P` inside Track Info runs the safe probe.

## Read / Write / Voice

1. `READ` opens small and large TXT books.
2. Line/page navigation works.
3. Speed mode opens with `1`.
4. `WRITE` creates, wraps, saves, reopens, edits, and deletes LAT notes.
5. Cyrillic notes open view-only.
6. `VOICE` records about 20 seconds, saves, plays, and deletes.
7. After Voice save/play, `READ`, `WRITE`, and `LISTEN` still show their SD files.

## Inbox / Persistence

1. Open a book, play a track, save a note, and save a Voice recording.
2. Return to launcher, then open `INBOX`.
3. Timeline shows `READ`, `LISTEN`, `NOTE`, and `VOICE` events.
4. Reboot and reopen `INBOX`; events remain visible.
5. `1` refreshes the internal journal without changing SD availability.

## Utilities

1. `TIME` stopwatch, timer preset, and alarm work.
2. `FILES` shows `TRANSFER`, known files open, unsupported files show `FILE INFO`.
3. `ROUTINES` toggles items, `1` advances day, stats/manage work.
4. `SETTINGS` shows SD/BAT/Transfer password, About, and SD reprobe.
5. `D` or `0` opens Dashboard; OK resumes, `1/2/3` shortcuts work.

## Transfer

1. `TRANSFER` starts AP.
2. Network `ABVX-Cardputer` appears.
3. Password is `cardputer`.
4. `http://192.168.4.1/api/ping` returns `ping ok`.
5. `/api/list?path=/music` lists files.
6. `/api/download?path=/notes/NOTE0001.TXT` downloads when the file exists.
7. Connections v3: upload TXT, 500-900 KB book, 5-10 MB MP3, then interrupt one upload; verify AP and SD remain available after every case.
8. Time sync: run `python3 tools/cardputer_time_sync.py sync`, confirm `OK TIME APPLIED`, then compare Time with the Mac clock.

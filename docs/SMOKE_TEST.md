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
7. Large upload remains out of scope; use SD reader for media.

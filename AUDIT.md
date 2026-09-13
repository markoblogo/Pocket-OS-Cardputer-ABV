# ABVx Release Audit

Scope: first-party firmware, build/configuration, simulator, upload tool, and project documentation. Vendored M5Unified, M5GFX, minimp3, and ESP-IDF are treated as privileged dependency boundaries.

## Baseline

- Target: M5Stack Cardputer ADV / ESP32-S3FN8.
- ESP-IDF: 5.4.x maintenance line.
- Baseline and hardened firmware builds succeeded.
- Simulator builds with `-Wall -Wextra`.
- Python uploader passes bytecode compilation.
- Hardware verified: Music playback and one 20-second Voice recording with SD files remaining available.

## Hardening completed

- Removed hidden Agent runtime paths.
- Voice now has one RAM-first 20-second mode; live SD recording is disabled and removed from the release surface.
- Message return routing uses one typed state instead of four stale flags.
- AP password is random for each Transfer session and displayed only while active.
- AP accepts one station at a time.
- Direct `/api/upload` remains disabled. Connections v3 staged upload accepts
  files up to 32 MB in bounded 2 KB chunks through the main-loop storage handoff.
- Staged upload uses explicit begin/chunk/finish/abort routes, inactivity timeouts,
  temporary files, final size verification, and no-overwrite publication.
- Upload client bounds and sanitizes HTTP responses.
- Simulator sanitizes SD-derived filenames before terminal output.
- Notes and recordings fail closed on filename exhaustion instead of overwriting existing files.
- WAV playback accepts only mono PCM, 8/16-bit, 8-48 kHz.
- SD-derived app lists and state collections have explicit entry caps.
- Download filenames are sanitized before use in HTTP headers.

## Current security model

Transfer is manually enabled and creates a WPA2 AP. The random password shown on Cardputer is the admission credential. HTTP is intentionally local and unencrypted inside that temporary AP. Stopping Transfer shuts down HTTP and Wi-Fi.

Physical possession, removable-SD confidentiality, secure boot, flash encryption, and trusted build workstation integrity remain deployment assumptions rather than application guarantees.

## Residual risks

- `main/main.cpp` remains monolithic. Split input, storage, audio, transfer, and UI only through hardware-tested incremental refactors.
- SD hot removal and electrical faults still require manual Settings -> SD Reprobe.
- Text files and directory lists are bounded, but malicious media safety still depends partly on FatFs, minimp3, and M5 libraries.
- The staged path is queued to the main loop and still requires
  interrupted-transfer hardware stress testing.
- No automated hardware test harness exists.
- Clock resets after full power-off until Companion/network time sync exists.

## Release smoke test

1. Boot and launcher navigation.
2. Music play/stop, waveform, volume, shuffle.
3. Voice record for 20 seconds, auto-save, playback, delete.
4. Verify Music, Read, Write, Voice, and Files still see SD.
5. Reader EN/RU book, speed mode, bookmark reopen.
6. Notes create/edit/delete and Cyrillic view-only behavior.
7. Time, Files, Routines, Decide, Settings.
8. Transfer: generated password, ping, list, download, staged 500-900 KB and
   5-10 MB uploads, interrupted upload recovery, GO shutdown.

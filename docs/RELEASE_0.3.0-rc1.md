# 0.3.0-rc1

Pre-release: data protection, Journey freshness and desktop Companion.

- Non-destructive mount failure behavior and Inbox backup recovery.
- Staged, content-deduplicated music/book sync with index recovery.
- Internal Voice HTTP export with hash checks; originals retained.
- Optional host transcription setup, not bundled model weights.
- GNSS fix freshness and guarded Journey CSV updates.
- Native Mac Companion installer, compact terminal-style UI and ABVx icon.
- Connection guidance separates USB, SD reader and Transfer Wi-Fi.

Development evidence: host checks and ESP-IDF compilation passed earlier in this
work. No new full test pass is claimed for the final UI/icon packaging changes.
Physical GNSS, live Voice Wi-Fi and real model transcription remain separate gates.

Firmware asset offsets (ESP32-S3, 8 MiB flash): bootloader at 0x0,
partition table at 0x8000, application at 0x10000. Back up first. Do not erase
flash or write a full-flash image over internal recordings. The release contains
individual firmware binaries, not a private device backup.

The Mac source/setup archive includes the native installer and Python source,
not a universal, Python-bundled or notarized app. It requires a local Python 3
installation and Apple command-line build tools. Firmware actions additionally
require this repository and ESP-IDF 5.4.2. Refer to docs/COMPANION_DESKTOP.md.

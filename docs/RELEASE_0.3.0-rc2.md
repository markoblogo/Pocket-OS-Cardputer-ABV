# 0.3.0-rc2 draft release notes

This candidate focuses on maintenance and reproducibility rather than new apps.

- Corrects the public Transfer, upload, security and licensing contracts.
- Generates a fresh Transfer password for each session.
- Updates the supported toolchain to ESP-IDF 5.4.4.
- Updates vendored M5GFX to 0.2.28 and M5Unified to 0.2.21.
- Pins CI actions, treats host C++ warnings as errors and enforces a 1.75 MiB
  firmware budget.
- Produces checksummed firmware assets and verifies packaged Companion resources.

Software evidence: host checks and clean ESP-IDF 5.4.4 builds pass on the staged
changes. Physical display, keyboard, SD, audio, Voice Transfer, GNSS, endurance
and power checks remain open in `HARDWARE_ACCEPTANCE.md`.

This release must remain a pre-release. Do not publish stable `v0.3.0`, M5Burner
catalog metadata or real-device claims until the hardware matrix is complete.

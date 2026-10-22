> Historical pre-rc3 review. Current findings: AUDIT_2026-10-22.md.

# Practical review: 2026-10-21

This is a focused review of the current firmware, SD publication path, build
workflow and candidate donors. It is not a full security audit or a hardware
acceptance report. The user's SD card was physically damaged while inserting it
into a reader; that incident is not evidence of a firmware/storage defect.

## Findings and decisions

1. Games needs actual screen acceptance. Tetris currently fits a 10x20 board into
   5-pixel cells on the landscape display. The requested next behavior is portrait
   play with rotated controls and restored orientation on exit. Klondike only
   moves top cards, draws at most five cards per column, clips its rightmost
   foundation and assigns red suits inconsistently with its CDHS labels.
   DOOM LITE starts at (2.5, 2.5), inside a wall, and increments kills on fire
   without targets. These are unfinished game prototypes, not completed ports.
2. Firmware coupling makes changes harder. Games state, rules, input, rendering
   and timers have now been extracted into games_app.cpp. main.cpp retains the
   Launcher boundary and shared hardware. Move other applications when changing
   them; avoid a wholesale rewrite or extra runtime.
3. SD publication already has useful safeguards: streaming hashes, verified new
   payloads, index-last publication and retained recovery files. Performance work
   should address repeated full-library scans/hashes and batch imports without
   weakening verification. Add measured progress, recovery-space accounting and
   an explicit verified-generation prune before considering automatic cleanup.
4. GNSS is not hardware-qualified. UART byte count zero does not distinguish
   firmware setup, power, contacts and a faulty receiver. First establish raw
   serial output with the exact CAP revision, then NMEA validity, then outdoor
   fix. A working LoRa radio alone does not prove its separate GNSS receiver.
5. Release evidence needs reconciliation: local version/status still says
   0.3.0-rc1 while Games came later. Record an identifiable build, device results,
   repeatable Companion setup and one consistent status before a stable release.
   A flash write/hash check does not establish screen or gameplay behavior.

## Donors found during current research

These are current candidates, not a claim that all appeared recently. Features
below are documented by their authors and have not been tested on this device.

| Candidate | Useful scope | Integration boundary |
| --- | --- | --- |
| [MAXXTANG/CardPuterAdvancedDoom](https://github.com/MAXXTANG/CardPuterAdvancedDoom) | Actual Doom ADV port, TCA8418 keyboard, ES8311 audio | Author describes an embedded IWAD and custom 6 MB app layout; our app partition is 4 MB with internal Voice storage. Qualify memory, asset licensing and storage layout before integration. Do not flash its partition table over our data casually. |
| [DevinWatson/Cardputer-Adv-GPS-Info](https://github.com/DevinWatson/Cardputer-Adv-GPS-Info) | CAP LoRa-1262 UART configuration, NMEA monitor, track preview | Compare pin ownership and SPI handling first; confirm source licensing before copying. A different UART controller number alone does not prove the cause of our issue. |
| [panpanweijie/cardputer-adv-sport-companion](https://github.com/panpanweijie/cardputer-adv-sport-companion) | Pause/resume/mark workout, CSV/GPX export, bounded route preview | Author calls it an experimental prototype tested on one setup. Use as a workflow reference; source availability and reusable implementation still require qualification. |
| [HorseyofCoursey/EMBER](https://github.com/HorseyofCoursey/EMBER) | ADV music browsing, seeking, themes and codec reference | No-PSRAM firmware separates visualizers and network streaming into different builds. Borrow narrowly and measure heap/audio behavior. |
| [jblanked/Picoware](https://github.com/jblanked/Picoware) | Desktop simulator, Companion and app distribution ideas | General feature list spans multiple boards. It is not proof every feature works on ADV; GPL and runtime dependencies need review before code adoption. |

The [official M5 CAP tutorial](https://docs.m5stack.com/en/arduino/projects/cap/cap_lora868)
provides a useful independent GPS example (RX 15, TX 13, 115200 baud for the
documented CAP). Confirm the actual module before electrical tests or wiring.

## Execution order agreed with the user

1. Finish module extraction, build/checks and Pocket OS flashing. Confirm the
   Launcher and Games navigation on the device.
2. Prepare the replacement SD from the Mac source library. Test one track/book,
   then publish the full library and verify index/payload consistency.
3. Reconnect and diagnose the exact GPS module with raw UART evidence; obtain a
   real fix before extending sports metrics. Then test Journey with Music.
4. Improve Games in separate slices: portrait Tetris, usable Klondike selection
   and layout, then a measured real Doom integration experiment.
5. Close Voice export/transcription acceptance and consolidate release docs.
   Optional Muse/agent work remains behind the everyday media/capture workflow.

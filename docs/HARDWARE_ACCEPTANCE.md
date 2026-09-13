# Hardware acceptance: 0.3.0-rc2

Status: **OPEN**. Host tests and compilation do not close this gate.

Use a backed-up M5Stack Cardputer ADV and disposable or fully backed-up storage.
Record the firmware commit, board revision, GNSS module, SD card, test date and
evidence for every row. Never publish private recordings, tracks or credentials.

| Gate | Acceptance evidence | Status |
| --- | --- | --- |
| Boot/input/display | Cold boot, launcher navigation, text entry and every primary screen | Pending |
| Music/Reader/SD | Unicode library, playback, navigation, bookmark and SD reprobe | Pending |
| Voice | Three recordings, playback, verified Wi-Fi export, second export copies zero, originals retained | Pending |
| Transfer | New displayed password after restart; direct and staged uploads; interrupted upload recovery | Pending |
| Persistence | Inbox recovery across reboot and controlled power interruption without auto-format | Pending |
| GNSS/Journey | UART/NMEA/outdoor fix, stale-fix rejection, reconnect without bridged points | Pending |
| Endurance | Journey plus Music for 20-30 minutes; clean stop, closed CSV, plausible route distance | Pending |
| Power | Settled battery observations with USB disconnected, with and without Music/GNSS load | Pending |

For each completed row, attach sanitized logs or photos to the tracking issue and
link them here. Record failures as failures; do not convert partial evidence into
`Passed`. The detailed steps remain in `SMOKE_TEST.md`.

Stable `v0.3.0`, real-device demo footage and M5Burner publication are allowed
only after all rows pass on the release commit.

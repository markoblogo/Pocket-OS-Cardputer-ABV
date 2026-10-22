# Hardware acceptance: 0.3.0-rc3

Status: **OPEN**. Host tests and compilation do not close this gate.

Use a backed-up M5Stack Cardputer ADV and disposable or fully backed-up storage.
Record the firmware commit, board revision, GNSS module, SD card, test date and
evidence for every row. Never publish private recordings, tracks or credentials.

| Gate | Acceptance evidence | Status |
| --- | --- | --- |
| Boot/input/display | Cold boot, launcher navigation, text entry and every primary screen | Pending |
| Games | Portrait Tetris input/hold/pause/exit, Klondike run move, Doom hit/exit; normal landscape and media after exit | Pending |
| Music/Reader/SD | Unicode library, playback, navigation, bookmark and SD reprobe | Pending |
| Voice | Three recordings, playback, verified Wi-Fi export, second export copies zero, originals retained | Pending |
| Transfer | New displayed password after restart; staged small/large uploads; interrupted upload recovery | Pending |
| Persistence | Inbox recovery across reboot and controlled power interruption without auto-format | Pending |
| GNSS/Journey | UART/NMEA/outdoor fix, stale-fix rejection, reconnect without bridged points | Paused by owner |
| Endurance | Core media/capture session; Journey plus Music separately after GNSS works | Pending |
| Power | Settled battery observations with USB disconnected, with and without Music/GNSS load | Pending |

For each completed row, attach sanitized logs or photos to the tracking issue and
link them here. Record failures as failures; do not convert partial evidence into
`Passed`. The detailed steps remain in `SMOKE_TEST.md`.

Stable `v0.3.0` and M5Burner publication require their release gates. A demo of
checked Music/Reader/Games paths can be filmed while experimental GPS is paused;
it must identify the candidate and avoid claims about untested features.

## Earlier development build observations (not rc3 acceptance)

2026-10-21, owner reports and direct diagnostics, on a pre-rc3 development build:

- Launcher and Games menu opened after the module extraction.
- Replacement FAT32 SD passed filesystem checks, write/read probe and library
  hash verification; owner confirmed Music playback and Reader opening books.
- CAP's I2C expander acknowledged at 0x43. Isolated UART diagnostics received
  zero bytes at 115200/9600/38400/57600/230400/4800 baud and no version reply.
  This narrows investigation to the GNSS communication/power/contact path; it
  does not prove the whole CAP is defective. GPS work was paused on 2026-10-22.
- The previous card was physically broken while inserting it into a reader.
  No firmware-induced SD damage is inferred.

These observations do not validate new rc3 game graphics or upgraded vendor code.

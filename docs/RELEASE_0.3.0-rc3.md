# Pocket OS 0.3.0-rc3: Games and release cleanup

A pre-release for **M5Stack Cardputer ADV**, 8 MB flash, no PSRAM.

- Portrait Tetris with 10px cells, seven-piece bag, next/ghost preview, pause,
  held-arrow repeat and landscape restoration on exit.
- Klondike with correct suit colours, sequence moves, stock recycling and
  scrolling column selection.
- Colour DOOM LITE with walls, enemies, collision, real hit detection, ammo,
  contact damage and a reachable exit. Original raycast adaptation, not DOOM/WAD.
- Separate, host-testable game rules and game rendering; one framebuffer.
- Current rc2 dependency/security/YTMamp baseline retained; documentation and
  installation asset claims reconciled.
- Companion marks imports and note backups busy, blocks competing jobs and
  refuses shutdown during transfer; loopback HTTP regression covers the state.

## Install

Download `cardputer-abvx-minimal.bin`, `bootloader.bin`, `partition-table.bin`
and `SHA256SUMS.txt`. Verify checksums and follow INSTALL.md after backing up
internal recordings/settings. No Voice partition image or merged whole-flash
image is supplied. Do not erase flash for an ordinary upgrade.

The optional source/Companion archive contains canonical host tools and local
Mac installer; it is not a standalone Python runtime or a notarized app.

## Evidence and limits

Software tests, render fixtures, firmware compilation and CI results are distinct
from physical-device acceptance. Check the associated commit's Checks workflow.
Games and other new rc3 paths require the hardware checklist; previous media
observations are recorded separately. GPS is paused after direct UART diagnostics
received zero bytes. This release does not claim working satellite tracking.

M5Burner/stable promotion remains pending. Software preview images are labelled
as renders. The next demo will show the actual tested device features.

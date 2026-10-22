# Project status: 0.3.0-rc3

This candidate brings the previous Games extraction onto the current main branch,
including rc2's dependency, Transfer-security, packaging and YTMamp changes.

Current apps: Music, Reader, internal Voice, Notes, Inbox, Time, Routines, Files,
Transfer, Games and optional YTMamp control. The Games rules are isolated from
rendering: portrait Tetris, sequence-aware Klondike and colour DOOM LITE. The
original Doom engine is not included.

Host behavior checks, renderer bounds and firmware compilation are required
before publication. CI uses ESP-IDF 5.4.4 and enforces the 1.75 MiB app budget.
The release remains a pre-release: software checks do not close device gates.

GPS work is paused by the owner. Direct UART diagnostics on the tested CAP
produced zero bytes at six speeds while its I2C expander acknowledged. Journey
is experimental; no outdoor fix or sports-tracking accuracy is claimed.

Prior device observations and new candidate acceptance are recorded only in
HARDWARE_ACCEPTANCE.md. Voice export/ASR quality, interrupted-write recovery,
endurance and the new Games hardware paths still need their stated checks.
M5Burner catalog/stable publication remains separate from this GitHub candidate.

Next: device smoke checks for rc3 and an honest Music/Reader/Games demo, following
DEMO_PLAN.md. GPS need not appear in a demo of the working media and Games paths.

See AUDIT_2026-10-22.md, GAMES.md, DATA_SAFETY.md and INSTALL.md for details.

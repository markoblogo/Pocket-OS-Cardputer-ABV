# Roadmap

## 1. Protect data

Candidate implements mount protection, recoverable Inbox and safe incremental SD
publication. Next: power-cut/card-removal hardware acceptance; then measured
recovery-folder pruning. Keep originals and previous good generations.

## 2. Complete Voice -> Mac -> text

Candidate implements internal Voice inventory/download, verified offload and real
optional Parakeet adapter. Next: actual recordings on the user's Mac, evaluate
Russian/Ukrainian/English short notes and street noise. Long recording, live
streaming and higher sample rates follow only if the short-note flow is useful.

## 3. Finish Journey + Music

Freshness and zero-quality fix rejection, 5-second CSV sampling, jump rejection
and checked close are implemented. Next: identify physical module/pins/power,
obtain raw NMEA and outdoor fix, then a 20-30 minute outing with music. Treat pace
and distance as experimental until compared with a reference route.

## 4. Ship a reproducible candidate

Reconcile main and development, run CI, include checksummed binaries, install and
backup instructions, generate Companion resources from canonical tools. Promote
to stable only after hardware evidence. Choose a project-wide license separately;
retain all donor notices.

## 5. Improve discovery

Concise README, illustrated workflow video, issue template and release page.
Capture real device footage next. Publish in M5Burner after the hardware gate.
Then share an actual use case with the Cardputer community.

## Later

Prepared-first Browser, Reader/Notes usability, optional host routing and final
visual polish remain secondary. Meshtastic is not planned. Firmware chat and
nano-LLM experiments must not displace capture, media, data safety or Journey.
# Games direction

The Launcher now has a `GAMES` section with playable Tetris, Klondike and a
compact `DOOM LITE` raycast prototype.
Tetris uses the existing Cardputer keyboard and returns to the Games menu without
rebooting. Controls are left/right to move, up to rotate, down to accelerate,
Enter to hard-drop, and Go/Back to leave.

Next game slices remain separate:

- Klondike: keyboard-only selection model using arrows, Enter and Go.
- DOOM LITE: keyboard-only raycast prototype, no SD/WAD dependency, no audio.
- Full DOOM: still a separate donor integration experiment. The direct
  Cardputer-doom donor is a proof-of-concept that expects PSRAM and a WAD
  partition, so it cannot be copied into this 8 MB no-PSRAM firmware unchanged.

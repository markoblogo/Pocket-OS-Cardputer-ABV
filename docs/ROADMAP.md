# Roadmap

## Current: rc3 and a useful demo

Ship tested Games rules, readable layouts and a reproducible pre-release. Verify
portrait transitions and keyboard repeat on Cardputer. Record Music, Reader and
Games on the physical device; label any software illustrations accurately.

## Reliability

Complete verified internal Voice export and short-note transcription on the
owner's Mac. Exercise interrupted writes and storage recovery on backed-up
media. Recovery-folder pruning needs a measured retention policy; do not delete
older generations automatically just to reclaim space.

## Maintainability

Keep rules and renderers separate. Games is the first application module; move
another screen only when modifying its behavior. Preserve main-loop ownership
of audio, storage and network handoffs. Maintain the firmware size budget and
checksummed release packaging.

## GPS: paused

The CAP's expander answers at 0x43; direct GNSS UART tests received zero bytes.
Resume only after reseating/contact checks and, if needed, independent power or
USB-UART measurements. Raw NMEA comes before outdoor fix and Journey + Music.
Meshtastic is not planned.

## Later

Prepared-first Browser, more Reader/Notes polish, optional host routing and final
artwork. A real no-PSRAM ADV Doom donor exists, but its larger app partition,
engine/WAD licenses and memory lifecycle need separate qualification. It is not
a drop-in replacement for the integrated DOOM LITE game or the Voice layout.

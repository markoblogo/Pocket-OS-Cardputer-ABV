# Project status: 0.3.0-rc1

The previous ADV baseline has real user experience with Music/Reader/Notes/Voice
and Connections/SD flows. This release candidate adds new code requiring new
acceptance; old hardware results are not silently applied to it.

Implemented: non-destructive Voice mount failure, blank-partition initialization,
Inbox generation recovery, index-last content-hash sync, shared storage lock,
source-first UI imports, internal Voice API/offload, optional Parakeet batch ASR,
fresh-fix Journey logging and checked Stop. Host tests and ESP-IDF build passed.

Pending evidence: flash this candidate, export actual internal recordings, real
ASR quality/timing, power-cut recovery on SPIFFS/FAT, GNSS UART/fix on the physical
module, 20-30 minute Journey + Music outing. Last known GNSS hardware observation
was UART B0; receiving coordinates has not been demonstrated in this task.

Release is a candidate, not stable. CI provides host checks and firmware artifacts.
M5Burner publication and a real device video wait for the hardware gate. The
included workflow video is explicitly an illustration.

See DATA_SAFETY.md, VOICE_TO_TEXT.md, INSTALL.md and SMOKE_TEST.md for current
contracts. Older design documents describe proposals, not proof of implementation.

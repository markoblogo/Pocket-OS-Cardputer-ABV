# Recording architecture

The current firmware captures up to 20 seconds of mono PCM WAV at **4 kHz/8-bit**
(about 80 KB). Earlier documents said 8 kHz; the code's current constants are 4 kHz.
Capture is RAM-first. Saving to internal `/voice` SPIFFS happens after Mic.end().
SD is not the current Voice store. Failed saves remain visible to the user.

The ADV has no external PSRAM. Earlier live SD capture caused EIO/SD instability,
so an always-streaming donor cannot be substituted without hardware validation.
The new host workflow is documented in VOICE_TO_TEXT.md: read-only export,
SHA-256 verification, optional local ASR, durable Markdown alongside original WAV.

Long recording is a separate experiment: bounded SD writer, actual byte-count
checks, recovery after power loss, and network work decoupled from recording.
Acceptance must include full SD, missing SD, slow Wi-Fi, sleep and low power.
A ring buffer alone does not prove this behavior.

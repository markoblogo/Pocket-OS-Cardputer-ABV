# Mac Companion References

Future ABVx Mac Companion should be a small local utility for preparing and syncing Cardputer SD content. It is not part of the firmware MVP.

## Target Mac Companion scope

- Books: import EPUB/FB2/PDF/DOCX/TXT, convert to clean TXT, split/normalize if needed, copy to `/books`.
- Music: import common audio formats, convert to Cardputer-friendly MP3, enforce safe filenames, copy to `/music`.
- Notes: pull/edit/export `/notes`, push plain TXT back.
- Recordings: pull `/rec` and `/RECS`, preview/export WAV.
- Device: SD status, time sync, config editing, firmware update later.

Initial transport should be SD reader / mounted volume. Wi-Fi transfer can be added after firmware transfer is stable.

## BookOrbit reference

Repository: https://github.com/bookorbit/bookorbit

BookOrbit is a self-hosted digital library platform for ebooks, PDFs, audiobooks, comics, metadata, OPDS, Kobo/KOReader sync, and web reading.

Useful as reference for ABVx:

- Library model: books, audiobooks, metadata, reading state.
- Import/drop-folder workflow: stage files, normalize, then finalize into a library.
- Multi-format thinking: EPUB/MOBI/AZW3/PDF/audio handling.
- Metadata enrichment concepts.
- OPDS / KOReader ideas for long-term reader ecosystem compatibility.
- UX pattern: user drops messy source files; app prepares device-ready output.

Not recommended as direct base for ABVx Mac Companion:

- It is a large server/web platform, not a lightweight Mac companion.
- Stack and runtime are much heavier than needed for local SD preparation.
- It solves multi-user/self-hosted library management, while ABVx needs single-user file preparation and device sync.
- It is licensed AGPL-3.0, so direct code reuse requires careful license compliance.

Decision:

- Use BookOrbit as product/architecture reference only.
- Do not copy code into ABVx without explicit licensing review.
- Build ABVx Mac Companion as a small focused app first.

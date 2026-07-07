# ABVx Roadmap

Product direction: offline pocket utility shell first, transfer second, browser and AI later.

## Phase 1: Stability baseline

Status: mostly complete.

Remaining work:

- Continue smoke testing Music → Record → Reader → Notes → Files after SD operations.
- Keep large Wi-Fi upload disabled until a separate transfer redesign is done.
- Tag a first test release after one clean full hardware pass.

## Phase 2: Offline app polish

Most valuable near-term work:

1. Record v2
   - Longer recording architecture.
   - Playback progress.
   - Better duration display.
   - More robust storage failure messages.

2. Reader v2
   - Better speed-reading UI.
   - More bookmark controls.
   - Last-book startup shortcut.

3. Notes v2
   - Rename notes.
   - Better line wrapping for long text.
   - Safer delete/restore pattern if useful.

4. Files v2
   - Details polish.
   - `/cardputer` transfer folder workflow.
   - Optional delete safeguards.

5. Time/Habits polish
   - Timer presets.
   - Cleaner habit summaries.
   - Future date/time sync hook.

## Phase 3: Connections / Transfer

Goal: Cardputer as SD transfer device via Wi-Fi AP.

Planned redesign:

- Keep current list/download endpoints.
- Replace direct large upload with explicit staged/chunked state machine.
- Process SD writes from main loop, not HTTP handler.
- Add phone-friendly web UI.
- Keep 8.3 filename restriction until FATFS LFN is intentionally enabled.

Acceptance:

- Upload MP3/TXT/WAV-sized files without AP drop or SD loss.
- Failed upload must not corrupt SD state.
- `/cardputer` works as generic transfer folder.

## Phase 4: Text Browser MVP

Goal: prepared-first text browser for a small set of favorite sites, not a graphical browser. See `docs/BROWSER_ARCHITECTURE.md`.

- Wi-Fi client mode.
- URL input.
- Favorites list.
- Mac-prepared favorite packages first.
- HTML-to-text extraction primarily in Mac Companion.
- Link list navigation.
- Offline cache for favorite pages.
- Downloads to `/books`, `/notes`, `/music`, or `/cardputer`.
- Keep Browser, Search, and AI Chat separate: curated pages, text-only search results, and general online Q&A.

## Phase 4.5: Mac Companion

Goal: local Mac tool for preparing and moving files to Cardputer SD.

- Import books and convert to clean TXT for `/books`.
- Import music and convert/rename to Cardputer-friendly MP3 for `/music`.
- Pull notes and recordings from SD.
- Sync time/config later.
- Use BookOrbit as reference for library/import ideas, not as direct base. See `docs/MAC_COMPANION_REFERENCES.md`.

## Phase 5: Agent / AI

Agent returns only when it adds value beyond launcher.

Offline Agent:

- Deterministic command router.
- Local memory cards.
- App actions: open reader, new note, today habits, timer, play music, status.

Online AI:

- OpenAI API when connected.
- Text question → text answer.
- Save answer to Notes.
- Voice input/output later.

## Out of scope

- Agro dashboards or market terminal positioning.
- Subscriptions or paid API model inside this firmware.
- Meshtastic integration.
- Full graphical browser.

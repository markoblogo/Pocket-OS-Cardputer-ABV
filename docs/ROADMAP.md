# ABVx Roadmap

Product direction: **Pocket OS** first, individual apps second.

ABVx should become a fast offline-first personal tool organized around actions:

```text
Capture / Remember / Read / Listen / Act / Reflect
```

Architecture:

- Product architecture: `docs/PRODUCT_ARCHITECTURE.md`
- Architecture decisions: `docs/ARCHITECTURE_DECISIONS.md`
- Browser architecture: `docs/BROWSER_ARCHITECTURE.md`
- Visual layer: `docs/VISUAL_LAYER_ARCHITECTURE.md`

## Phase 1: Stability baseline

Status: mostly complete.

Remaining work:

- Continue smoke testing Music -> Record -> Reader -> Notes -> Files after SD operations.
- Keep large Wi-Fi upload disabled until a separate transfer redesign is done.
- Tag a first test release after one clean full hardware pass.

Acceptance:

- Music plays smoothly.
- Record creates, saves, plays, and deletes stable short voice notes.
- Reader/Notes/Files keep seeing SD after Music/Record operations.
- Settings SD reprobe works.

## Phase 2: Pocket OS foundation

Goal: turn the app set into a fast personal operating layer.

### 2.1 One Button Capture

Status: MVP implemented from launcher.

- `R`: start voice recording.
- `N`: new note.
- `M`: play selected music.
- `T`: timer shortcut later.
- Fn combinations remain optional/later.

Next acceptance:

- Hardware-test launcher shortcuts.
- Add Inbox logging after shortcuts are stable.

### 2.2 Universal Inbox

- Add `/sdcard/inbox/INBOX.TXT`.
- Log voice saves, note saves, habit checks, reading opens, music plays, timer events.
- Start as append-only text log.

Acceptance:

- Captured activity appears in a single file.
- Inbox survives reboot.

### 2.3 Context Resume

- Store last useful state: book, track, note, recording, timer/habits summary.
- Add Resume screen or Dashboard section.

Acceptance:

- User can return to last book/track/note quickly.

### 2.4 Fast Dashboard

- Boot/default screen with battery, resume state, habits count, current book/track, shortcuts.
- Launcher remains accessible via OK/Menu.

Acceptance:

- Boot gives useful state, not only app list.

### 2.5 Timeline

- Build a simple view over Inbox.
- Use manual day/session time until real sync exists.

Acceptance:

- User can review today's captured activity.

## Phase 3: Offline app polish

Apps stay as implementation modules under Pocket OS actions.

1. Record v2
   - Stable duration policy based on hardware test.
   - Playback progress.
   - Better duration display.
   - Robust storage failure messages.

2. Reader v2
   - Real English/Russian book tests.
   - Bookmark polish if test reveals gaps.
   - Last-book startup shortcut feeds Context Resume.

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

## Phase 4: Connections / Transfer

Goal: Cardputer as SD transfer device via Wi-Fi AP and later Mac Companion.

Current rule: large upload is disabled.

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

## Phase 5: Mac Companion

Goal: local Mac tool for preparing and moving content to Cardputer SD.

- Import books and convert to clean TXT for `/books`.
- Import music and convert/rename to Cardputer-friendly MP3 for `/music`.
- Prepare browser favorite packages.
- Pull notes and recordings from SD.
- Sync time/config.
- Proxy AI requests.
- Firmware update later.
- Use BookOrbit as reference for library/import ideas, not as direct base. See `docs/MAC_COMPANION_REFERENCES.md`.

## Phase 6: Prepared-first Browser

Goal: text browser for a small set of favorite sites, not a graphical browser.

- Mac-prepared favorite packages first.
- Cardputer reads cached text + links.
- Offline shows saved pages.
- Online refresh updates known favorites later.
- Text Search is an adapter, not a full Google browser.
- AI Chat handles general questions.

## Phase 7: AI / Command Surface

AI is online-only.

Preferred route:

- Cardputer -> Mac Companion -> OpenAI -> Cardputer.

Later/fallback:

- Cardputer -> OpenAI API directly.

MVP:

- Text input in English, French, or translit.
- Text answer displayed on Cardputer.
- Save answer to Notes.
- Voice input/output later.

No offline nano-LLM. No standalone Agent unless it becomes a real command/AI surface beyond launcher duplication.

## Phase 8: Adventure / GPS / LoRa

Goal: use the planned LoRa/GNSS Cardputer ADV module for offline-first outdoor sessions.

Architecture: `docs/ADVENTURE_LORA_ARCHITECTURE.md`

Implementation order:

- GNSS Lab: read NMEA, show fix/lat/lon/speed/altitude.
- Journey/Breadcrumb MVP: local GPS track in `/sdcard/journeys`.
- Running Mode: large distance/pace/time screen.
- Voice Marks: short recording attached to GPS/time.
- Mac Companion GPX export.
- LoRa Lab: PING/RSSI/SNR and duty-cycle-safe packets.
- Direct Computer Relay MVP: STATUS, GPS_POINT, WORKOUT_SUMMARY, ACK, TIME_SYNC with a computer-connected LoRa module.

Rules:

- GPS/Journey works fully offline.
- LoRa is sparse direct exchange with a computer/home station, not file transfer.
- Relay failure never blocks local capture.
- No Meshtastic, no mesh, no generic LoRa chat/networking in this product track.

Acceptance:

- A walk/run can produce a local journey folder with track data.
- Later, a computer-connected LoRa station can receive small status/checkpoint packets without SD/file sync.

## Final polish: Visual Layer

After core apps and companion workflows are stable, add Minimal/Art visual modes.

- Current UI remains default Minimal mode.
- Art mode is optional.
- Art mode is battery-gated and auto-falls back to Minimal when battery is low.
- Includes boot/sleep screens, transitions, cyberpunk accents, richer waveform/saver visuals.

## Out of scope

- Agro dashboards or market terminal positioning.
- Subscriptions or paid API model inside this firmware.
- Meshtastic integration.
- Full graphical browser.
- Offline fake AI / nano-LLM.

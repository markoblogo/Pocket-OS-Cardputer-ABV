# ABVx Pocket OS Product Architecture

ABVx is a Pocket OS for Cardputer ADV: a small offline-first personal operating layer for fast capture, memory, reading/listening, routines, and transfer.

## Product framing

Old framing:

```text
Music / Reader / Notes / Record / Time / Files
```

New framing:

```text
Capture / Remember / Read / Listen / Act / Reflect
```

Apps remain, but they become implementation modules behind human actions.

## Core principles

- Fast actions beat deep menus.
- Offline first; online is optional.
- Prepared content beats heavy on-device parsing.
- Physical keys should replace cursor navigation where possible.
- Every frequent action should be possible in a few key presses.
- Minimal UI is default; Art mode is optional and battery-gated.

## Action model

### Capture

Purpose: quickly save thoughts and voice.

Backed by:

- Record;
- Notes;
- future One Button Capture;
- future Universal Inbox.

### Remember

Purpose: preserve and review personal state.

Backed by:

- Notes;
- Habits;
- Timeline;
- Memory Cards later.

### Read

Purpose: consume prepared text.

Backed by:

- Reader;
- prepared Browser pages;
- Mac Companion conversion.

### Listen

Purpose: music and voice playback.

Backed by:

- Music;
- Record playback.

### Act

Purpose: small utility actions.

Backed by:

- Time;
- Timer;
- Randomizer;
- Files;
- future command/AI surface.

### Reflect

Purpose: review activity and progress.

Backed by:

- Habits stats;
- Timeline;
- Dashboard;
- future Mac Companion summaries.

## Signature product features

### One Button Capture

Target behavior:

```text
R -> start/stop voice recording
N -> new note
M -> music play/stop
T -> timer
```

If Fn combinations are reliable, use `Fn+R`, `Fn+N`, etc. If not, use direct keys from Dashboard/Launcher.

### Universal Inbox

A single event log for captured items:

```text
/sdcard/inbox/INBOX.TXT
```

Example entries:

```text
D00012 09:13 VOICE /rec/REC0012.WAV
D00012 09:24 NOTE /notes/NOTE0013.TXT
D00012 09:40 HABIT pushups done
D00012 10:05 READ /books/RU1.TXT
```

### Timeline

A view over Inbox and app state:

```text
TODAY
09:13 Voice
09:24 Note
09:40 Habit
10:05 Reading
```

Before real time sync, use manual day numbers and session time.

### Context Resume

Resume last useful state:

```text
RESUME
> Book RU1 p42
  Track A.MP3
  Note NOTE0021
  Timer 05:00
```

### Fast Dashboard

Future boot/default screen:

```text
ABVx       72%

NOW
Book RU1 42%
Track A.MP3
Habits 3/7
Notes 2 today

OK MENU   R REC   N NOTE
```

Launcher remains available, but dashboard becomes the fastest path to daily actions.

### Friction Counter

Development metric, not necessarily a user app.

Track how many key presses frequent actions require. If a common task takes too many steps, add a shortcut or dashboard action.

### Memory Cards

Later context layer:

```text
Running
Work
Travel
Ideas
```

Each card may hold preferred notes, timers, books, music, and habits.

### Progressive Apps

Every app starts simple:

```text
Open / Resume / Capture
```

Advanced functions are secondary, not first-screen clutter.

### Zero Cursor Philosophy

Use direct physical keys when possible:

- `Y` / `N` for confirmations.
- `R` for record.
- `N` for note.
- `M` for music.
- Number keys for modes.

Arrow navigation remains for real lists.

## System shape

```text
Dashboard
├─ Capture
│  ├─ Voice
│  ├─ Text
│  └─ Inbox
├─ Remember
│  ├─ Notes
│  ├─ Habits
│  └─ Timeline
├─ Read
│  ├─ Reader
│  └─ Prepared Browser
├─ Listen
│  ├─ Music
│  └─ Recordings
├─ Act
│  ├─ Time
│  ├─ Randomizer
│  └─ Files
├─ Transfer
│  └─ Connections / Mac Companion
└─ Settings
```

## Implementation strategy

Do not rewrite the firmware around the new framing immediately.

Layer the Pocket OS features over the existing stable apps:

1. Add One Button Capture.
2. Add Universal Inbox logging.
3. Add Context Resume.
4. Add Dashboard.
5. Add Timeline view.
6. Add Memory Cards later.
7. Add Art mode in final polish.

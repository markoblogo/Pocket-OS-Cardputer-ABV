# Firmware module boundaries

The firmware is being split incrementally. Keep application state, rules and
rendering together; keep hardware ownership and application switching explicit.
Do not introduce another runtime or rewrite working applications as part of a move.

## Current split

| File | Owns |
| --- | --- |
| main/main.cpp | Hardware setup, shared canvas/theme, keyboard, Launcher, audio/storage/network coordination, remaining application screens |
| main/games_app.h and main/games_app.cpp | Games menu, Tetris, Klondike and DOOM LITE state, rules, input handling, timed updates and rendering |
| main/games_core.h and main/games_core.cpp | Display-independent Tetris, Klondike and Doom rules, collision and state; host-tested |
| main/gnss_service.h and main/gnss_service.cpp | GNSS UART reception and NMEA/fix state |
| main/journey_service.h and main/journey_service.cpp | Journey CSV lifecycle, distance and elapsed time |
| main/persistence.h and main/persistence.cpp | Internal event-log and recovery helpers |

## Games contract

- Launcher calls games::enter(now_ms) and uses a single Games screen.
- Keyboard events are translated to games::Input. handleKey returns true only
  when Go/Back exits the Games menu; leaving a game returns to that menu first.
- update(now_ms) runs only while Games is foreground and requests a redraw when
  a timed update changes the game. The caller supplies the monotonic clock.
- draw receives the existing canvas and current palette. Games does not create
  another framebuffer or take ownership of SD, audio or GNSS.
- portrait() requests the explicit display boundary. The firmware releases and
  recreates its one 8-bit sprite at 135x240, or 240x135 when leaving Tetris.
- Rules live in games_core; Games views and rule instances stay in games_app.
  The caller supplies time; rules have no M5Unified or ESP-IDF dependency.

## Next small steps

1. Check portrait transitions and held-key repeat on the physical device.
2. Qualify a real ADV Doom donor separately before replacing the current adaptation.
3. Extract another application only when working on its behavior, using the same
   input/time/render boundary. Avoid a whole-firmware rewrite.

## Verification

Build with ESP-IDF 5.4.2 after moving module boundaries. On hardware, exercise
Launcher -> Games -> each game -> Games -> Launcher, reopen Games, switch theme,
and check that Music/Journey still behave as before. Compilation alone does not
establish keyboard or screen acceptance.

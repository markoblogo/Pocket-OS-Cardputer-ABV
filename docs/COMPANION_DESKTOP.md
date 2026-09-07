# Companion desktop: connection-first UI

The default screen contains connection guidance, Music, Books and Voice.
Firmware, note backup and time sync are under Additional actions. Experimental
AI and regex diagnostics are no longer exposed on the everyday screen. Existing
backend operations and confirmation handlers are retained, not reimplemented.

## Install on this Mac

Run `zsh tools/install_companion_app.zsh` once. It installs
`/Applications/ABVx Companion.app`. Open that app from Applications thereafter;
no Terminal or browser tab is needed. The native window hosts the local UI and
starts its private Python service on localhost port 18765. It refuses a normal
quit while a background job is running or its state cannot be determined.

The installer bundles the Python source and UI, but records the current Python
interpreter and repository path. This is a local installation, not a portable,
notarized distribution. Moving/removing Python requires reinstalling. Firmware
build and flash still require the checkout and ESP-IDF. The installer refuses
to overwrite an existing app; move the old app aside or supply another path.

## What connecting means

- USB: firmware connection. It does not expose the SD as a Mac disk.
- SD reader: music, books and notes. Detection refreshes in the UI.
- Transfer Wi-Fi: internal Voice export and device time. Open Transfer on the
  device, then join its network on the Mac. Legacy firmware may lack Voice API.
- Transcription: optional separate local model setup; installation of the app
  does not install model weights or claim successful live transcription.

Operations never start solely because a device is plugged in. Music and book
folder synchronization asks for confirmation. Voice originals are retained.
Wait for completion and eject SD in Finder before removing it.

## Manual acceptance

1. Launch from Applications, without a pre-existing browser session.
2. With USB only, explain that SD is absent and disable music/book transfer.
3. Attach a prepared SD: counters and import controls become available.
4. Test a temporary music/book fixture, then confirm index and payload results.
5. Without Transfer Wi-Fi, Voice must produce an error, never a success claim.
6. During a running job, quitting must not interrupt file writes.
7. Reopen after a clean quit and confirm the server starts again.

Logs: `~/Library/Application Support/ABVx Companion/desktop.log`.

## Compact terminal presentation

The desktop screen now uses a compact monospaced, high-contrast palette, square
borders and ASCII-style section markers. Disabled controls remain readable;
error toasts have explicit foreground/background colors. Initial SD state is
unknown until a status response arrives, rather than inferred from CSS classes.
Voice request acknowledgement is not described as completed offload. Wi-Fi
errors provide connection guidance while preserving the detailed operation log.
These changes do not establish a working device connection or install AI models.

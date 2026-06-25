# Real-Device Test Matrix

Use this matrix for the first Cardputer-Adv release-candidate pass.

| Area | Test | Expected Result |
|---|---|---|
| Boot | Boot with prepared SD | Main menu appears; no crash loop. |
| Screen | Let timeout expire | Backlight turns off; first input wakes only. |
| Keyboard | Input Test: letters/arrows/backspace/enter | Semantic events appear correctly. |
| BtnGO | Input Test: short press | `Select` event. |
| BtnGO | Input Test: hold 700 ms+ | `Back` long event/menu behavior. |
| SD mount | System Info | SD shows mounted. |
| Notes | Create/open/save note | `.txt` appears in `/notes`. |
| Reader normal | Open `.txt`, scroll lines/pages | Text scrolls without blocking. |
| Reader speed | Switch mode, adjust WPM/chunk | Center text updates. |
| Music | Play/pause/next/volume/shuffle | MP3 plays and controls work. |
| Music screen-off | Play music, wait for screen-off | Playback continues; first key wakes only. |
| Recorder | Start/pause/resume/stop/save | WAV file appears in `/recordings`. |
| Recorder screen-off | Record while screen off | Recording continues. |
| WAV validity | Open saved WAV on computer | File is valid PCM WAV. |
| Wi-Fi scan | Scan networks | SSIDs, lock/open marker, RSSI appear. |
| Wi-Fi connect | Enter password | Status reaches connected, IP shown. |
| Wi-Fi reconnect | Reboot/reconnect saved | Known network reconnect works. |
| Web list | GET `/api/list?path=/notes` | JSON directory listing. |
| Web upload | Upload file to `/notes` | File appears on SD. |
| Web download | Download file | File downloads intact. |
| Web delete | Delete test file | File removed; root delete rejected. |
| Browser fetch | Open URL/search | Text content appears. |
| Browser links | Open links view, select link | Selected link loads. |
| Browser back/save | Back, GO save | Saved page appears in `/browser/saved_pages`. |
| AI text | Configure endpoint and send prompt | Response text appears and saves to `/ai`. |
| Clock NTP | Connect Wi-Fi, open Clock | Time syncs to Europe/Paris. |
| Stopwatch | Start/pause/reset | Elapsed time correct. |
| Timer | Start short timer | Beep at finish. |
| Randomizer | Yes/No/Maybe | Result changes. |
| Randomizer number | Adjust range, roll | Number within range. |
| Randomizer list | Edit list file, roll | Item from file appears. |
| System Info | Open app | SD, heap, Wi-Fi, IP shown. |
| Input Test | Wake suppression | `Wake` appears, original input not forwarded. |

## Audio Validation Priority

MusicApp and RecorderApp need the most real hardware time:

- Verify M5 speaker output path and volume.
- Verify mic sample continuity.
- Avoid recording while music is playing.
- Timer beep may be delayed or suppressed while recording.


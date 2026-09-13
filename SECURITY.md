# Security policy

## Supported version

Security fixes currently target the newest published release candidate. Older
test builds and development snapshots are not supported.

## Reporting a vulnerability

Use GitHub's **Security → Report a vulnerability** private reporting flow. Do
not open a public issue for a vulnerability or attach voice recordings,
location tracks, Wi-Fi credentials, flash backups, books, or music files.

Include the firmware version, exact Cardputer model, reproduction steps, impact,
and whether physical access is required. Maintainers will acknowledge a useful
report within seven days and coordinate disclosure after a fix is available.

## Security boundary

Transfer is a manually started, temporary WPA2 access point with a generated
password and one connected station. Its HTTP traffic is not independently
encrypted. Physical access, removable-media confidentiality, secure boot, flash
encryption, and the integrity of the build Mac remain deployment assumptions.

The Mac Companion listens only on localhost and uses a per-launch token. It is
not a remotely exposed service and does not bundle model weights.

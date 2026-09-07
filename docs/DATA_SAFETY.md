# Data safety contract (0.3.0-rc1)

## Internal flash

Voice storage does not format on a mount error. First-use formatting is allowed
only after reading the entire Voice partition and proving every byte is erased
(0xFF). A non-empty, unmountable partition is retained for recovery.

Inbox writes a synced temporary generation, renames the committed log to `.BAK`,
then publishes the new log. If the log is missing, load restores `.BAK`. A partial
`.NEW` never replaces the committed history. SPIFFS power-cut testing is pending.

## Music and books

All source validation/preparation completes before publication. Invalid or empty
sources leave the previous library in place. Unchanged payload hashes retain
storage names; additions use unused 8.3 names and are fsynced and rehashed.
Only after all additions succeed is the new index published. The previous index
is retained as INDEX.BAK / BOOKS.BAK. Superseded payloads move to the hidden
`.abvx-retired` directory after publication, so they do not appear in device lists.
If the active index is missing, the next sync restores its backup and required
retired payloads before attempting a new publication.

Interrupted retirement may temporarily leave extra visible files; rerunning sync
finishes retirement. Do not manually delete `.abvx-retired` while recovering a
failed transfer. It intentionally consumes space; automatic pruning is deferred.
There is no whole-filesystem atomicity or guarantee against physical FAT damage.
A corrupted (rather than missing) index needs manual recovery from the backup.

Music identity is a whole-file SHA-256, not an acoustic fingerprint. Different
encodings/tags are distinct. Equal display titles do not suppress distinct audio.
Mac AppleDouble files are ignored. Drag/drop imports into the source library,
then uses the same prepared-mirror path. Conflicting source names fail visibly.

CLI pipeline and Companion imports use a cross-process storage lock. Always wait
for job success and safely eject the SD before unplugging. A count alone does not
prove content integrity; successful sync verifies copied payload hashes.

## Voice and flash

Read-only device endpoints expose internal WAVs only while Transfer is open.
The AP uses a generated password displayed on device (changes after reboot).
Companion verifies device hashes, writes durable content-addressed backups and
records the inventory only after every WAV is saved. Device files are not deleted.
Companion Flash runs a fresh offload first and fails closed if it cannot complete.
Legacy migration is documented separately in INSTALL.md.

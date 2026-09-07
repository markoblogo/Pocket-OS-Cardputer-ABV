import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import abvx_companion as core
import voice_companion as voice


def mp3(path, value=0):
    path.write_bytes(b'ID3\x03\0\0\0\0\0\0\xff\xfb\x90\0' + bytes([value]) * 128)


class Reliability(unittest.TestCase):
    def test_invalid_source_preserves_mirror(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp); src=root/'src'; src.mkdir(); mirror=root/'mirror'
            mp3(src/'one.mp3'); core.sync_music_mirror(src, mirror)
            before=(mirror/'music/INDEX.TXT').read_bytes()
            (src/'broken.mp3').write_bytes(b'ID3')
            with self.assertRaises(RuntimeError): core.sync_music_mirror(src, mirror)
            self.assertEqual(before,(mirror/'music/INDEX.TXT').read_bytes())
            self.assertTrue((mirror/'music/M001.MP3').exists())

    def test_noop_sync_and_title_collision(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp); src=root/'src'; src.mkdir(); mirror=root/'mirror'
            mp3(src/'Zh.mp3',2); mp3(src/'Ж.mp3',3)
            core.sync_music_mirror(src,mirror)
            target=root/'sd'; core.deploy_mirror_to_sd(target,mirror,'music')
            times={p.name:p.stat().st_mtime_ns for p in (target/'music').iterdir() if p.is_file()}
            core.deploy_mirror_to_sd(target,mirror,'music')
            self.assertEqual(times,{p.name:p.stat().st_mtime_ns for p in (target/'music').iterdir() if p.is_file()})
            self.assertEqual(list(core.read_index(target/'music/INDEX.TXT').values()), ["Zh", "Zh"])

    def test_interruption_keeps_old_index_and_payload(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp); src=root/'src'; src.mkdir(); mirror=root/'mirror'; sd=root/'sd'
            mp3(src/'one.mp3'); core.sync_music_mirror(src,mirror); core.deploy_mirror_to_sd(sd,mirror,'music')
            before=(sd/'music/INDEX.TXT').read_bytes()
            mp3(src/'two.mp3',2); core.sync_music_mirror(src,mirror)
            with patch.object(core,'atomic_copy',side_effect=OSError('unplugged')):
                with self.assertRaises(OSError): core.deploy_mirror_to_sd(sd,mirror,'music')
            self.assertEqual(before,(sd/'music/INDEX.TXT').read_bytes())
            self.assertTrue((sd/'music/M001.MP3').exists())
            core.deploy_mirror_to_sd(sd,mirror,'music')
            self.assertEqual(len(core.read_index(sd/'music/INDEX.TXT')),2)

    def test_retired_tracks_are_hidden_and_backup_recovers(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp); src=root/'src'; src.mkdir(); mirror=root/'mirror'; sd=root/'sd'
            mp3(src/'one.mp3'); core.sync_music_mirror(src,mirror); core.deploy_mirror_to_sd(sd,mirror,'music')
            (src/'one.mp3').unlink(); mp3(src/'two.mp3',2)
            core.sync_music_mirror(src,mirror); core.deploy_mirror_to_sd(sd,mirror,'music')
            self.assertEqual(len(core.visible_files(sd/'music','.mp3')),1)
            (sd/'music/INDEX.TXT').unlink()
            core.deploy_mirror_to_sd(sd,mirror,'music')
            for name in core.read_index(sd/'music/INDEX.TXT'): self.assertTrue((sd/'music'/name).exists())

    def test_voice_hash_retry_and_receipt(self):
        payload=b'RIFF'+b'\0'*4+b'WAVE'+b'\0'*40
        digest=hashlib.sha256(payload).hexdigest()
        manifest={'version':1,'device_id':'001122334455','files':[{'name':'REC00001.WAV','size':len(payload),'sha256':digest}]}
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp)
            with patch.object(voice,'fetch',side_effect=[json.dumps(manifest).encode(),b'wrong']):
                with self.assertRaises(RuntimeError): voice.offload(root)
            self.assertFalse((root/'001122334455/offload.json').exists())
            with patch.object(voice,'fetch',side_effect=[json.dumps(manifest).encode(),payload]): voice.offload(root)
            with patch.object(voice,'fetch',return_value=json.dumps(manifest).encode()) as get:
                voice.offload(root); self.assertEqual(get.call_count,1)
            self.assertTrue((root/'001122334455'/f'{digest}.wav').exists())

    def test_reject_voice_paths(self):
        manifest={'version':1,'device_id':'001122334455','files':[{'name':'../REC00001.WAV','size':44,'sha256':'0'*64}]}
        with patch.object(voice,'fetch',return_value=json.dumps(manifest).encode()):
            with self.assertRaises(RuntimeError): voice.inventory(voice.DEVICE_URL)


if __name__=='__main__': unittest.main()

import importlib.util
import tempfile
from pathlib import Path

MODULE_PATH = Path(__file__).parents[1] / 'tools' / 'prepare_music.py'
SPEC = importlib.util.spec_from_file_location('prepare_music', MODULE_PATH)
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


def test_transliteration():
    assert MODULE.display_title('Привет мир.mp3') == 'Privet mir'
    assert MODULE.display_title('שלום.mp3') == 'shlvm'


def test_copy_and_index():
    with tempfile.TemporaryDirectory() as root:
        root = Path(root)
        source = root / 'source'
        target = root / 'music'
        source.mkdir()
        target.mkdir()
        (source / 'Привет.mp3').write_bytes(b'ID3test')
        assert MODULE.copy_library(source, target) == 1
        assert (target / 'M001.MP3').read_bytes() == b'ID3test'
        assert 'M001.MP3|Privet' in (target / 'INDEX.TXT').read_text(encoding='utf-8')


if __name__ == '__main__':
    test_transliteration()
    test_copy_and_index()
    print('music prepare tests: OK')

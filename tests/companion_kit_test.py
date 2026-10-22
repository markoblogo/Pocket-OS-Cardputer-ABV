import hashlib
import importlib.util
from pathlib import Path
import tempfile
import zipfile
from unittest.mock import patch

root = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("kit", root / "tools/package_companion_kit.py")
kit = importlib.util.module_from_spec(spec)
spec.loader.exec_module(kit)
with tempfile.TemporaryDirectory() as directory:
    output = Path(directory)
    archive = kit.package(root, output)
    first = archive.read_bytes()
    with patch("zipfile.time.localtime", return_value=(2031, 1, 1, 0, 0, 0, 2, 1, 0)):
        kit.package(root, output)
    assert first == archive.read_bytes(), "kit must be reproducible"
    assert len((output / "SHA256SUMS.txt").read_text().splitlines()) == 1
    assert hashlib.sha256(first).hexdigest() in (output / "SHA256SUMS.txt").read_text()
    with zipfile.ZipFile(archive) as bundle:
        names = bundle.namelist()
        assert "ABVx-Companion/tools/install_companion_app.zsh" in names
        assert "ABVx-Companion/tools/abvx_companion_app.py" in names
        assert not any(".app/" in name or "__pycache__" in name for name in names)
        assert not any(name.endswith((".mp3", ".wav", ".cact")) for name in names)
print("Companion source kit: PASS")

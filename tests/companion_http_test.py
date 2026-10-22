"""Loopback HTTP smoke with disposable storage and no device writes."""
import importlib
import json
import os
from pathlib import Path
import sys
import tempfile
import threading
import urllib.error
import urllib.request
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
with tempfile.TemporaryDirectory() as directory:
    root = Path(directory)
    export = root / "export.sh"
    export.write_text("# synthetic SDK marker\n")
    with patch.dict(os.environ, {"ABVX_PROJECT_ROOT": str(root),
                                 "ABVX_BACKUP_ROOT": str(root / "backup"),
                                 "ABVX_IDF_EXPORT": str(export),
                                 "ABVX_INTENT_ADAPTER": "rule_based",
                                 "CARDPUTER_LOCAL_MODEL_URL": "http://127.0.0.1:1"}):
        app = importlib.import_module("abvx_companion_app")
        app.usb_ports = lambda: []
        app.SD_OVERRIDE = str(root / "absent")
        app.ensure_backup_catalog()
        server = app.ThreadingHTTPServer(("127.0.0.1", 0), app.Handler)
        worker = threading.Thread(target=server.serve_forever, daemon=True)
        worker.start()
        base = "http://127.0.0.1:" + str(server.server_address[1])

        def request(path, body=None, token=None, host=None):
            headers = {"Content-Type": "application/json"}
            if token:
                headers["X-ABVX-Token"] = token
            if host:
                headers["Host"] = host
            req = urllib.request.Request(base + path, data=body, headers=headers)
            try:
                response = urllib.request.urlopen(req, timeout=5)
            except urllib.error.HTTPError as error:
                response = error
            with response:
                return response.code, response.read()

        try:
            status, html = request("/")
            assert status == 200 and b"simpleCompanion" in html and b"__ABVX_TOKEN__" not in html
            status, data = request("/api/status")
            data = json.loads(data)
            assert status == 200 and not data["ready_import"] and not data["ready_build"]
            assert data["idf_ready"] and not data["project_ready"]
            sd = root / "sd"
            (sd / "music").mkdir(parents=True)
            (sd / "books").mkdir()
            (sd / "music/M001.MP3").write_bytes(b"ID3\0\xff\xfb\x90\0" + b"\0" * 128)
            (sd / "books/B0001.TXT").write_text("A synthetic book.\n")
            app.SD_OVERRIDE = str(sd)
            _, data = request("/api/status")
            data = json.loads(data)
            assert data["ready_import"] and data["sd"]["music"] == 1 and data["sd"]["books"] == 1
            assert request("/api/build", b"{}")[0] == 403
            assert request("/api/build", b"{}", "wrong-token")[0] == 403
            status, data = request("/api/build", b"{}", app.STATE.token)
            assert status == 400 and b"full Pocket OS checkout" in data
            assert request("/api/status", host="untrusted.example")[0] == 403
            assert app.STATE.snapshot()["state"] != "RUNNING"
        finally:
            server.shutdown()
            server.server_close()
            worker.join(timeout=5)
print("Companion HTTP smoke: PASS (UI, SD readiness, kit build guard, host/token checks)")

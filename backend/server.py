from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import urlparse, parse_qs
import json
import subprocess
import tempfile
import sys

ROOT = Path(__file__).resolve().parent.parent
EXE = Path(tempfile.gettempdir()) / "smart_hospital_api.exe"
SOURCES = [
    "backend/api_main.c",
    "backend/file_handler.c",
    "backend/linked_list.c",
    "backend/hash_table.c",
    "backend/patient_queue.c",
    "backend/priority_queue.c",
    "backend/resource_manager.c",
]

def compile_backend():
    command = [
        "gcc", "-std=c11", "-Wall", "-Wextra", "-Wpedantic",
        "-I", str(ROOT / "include"),
        *[str(ROOT / source) for source in SOURCES],
        "-o", str(EXE),
    ]
    result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
    if result.returncode != 0:
        print("C backend compilation failed:", file=sys.stderr)
        print(result.stdout, file=sys.stderr)
        print(result.stderr, file=sys.stderr)
        raise SystemExit(result.returncode)
    print("C backend compiled successfully.")

def run_c(*args):
    result = subprocess.run(
        [str(EXE), *[str(arg) for arg in args]],
        cwd=ROOT, capture_output=True, text=True
    )
    try:
        data = json.loads(result.stdout)
    except json.JSONDecodeError:
        return {"ok": False, "error": "C backend returned invalid output."}, 500
    if result.returncode == 0:
        return data, 200
    status = 404 if "not found" in data.get("error", "").lower() else 400
    if "already exists" in data.get("error", "").lower() or "already has" in data.get("error", "").lower():
        status = 409
    if "no beds" in data.get("error", "").lower():
        status = 409
    return data, status

class Handler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(ROOT), **kwargs)

    def send_json(self, data, status=200):
        payload = json.dumps(data).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(payload)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(payload)

    def do_GET(self):
        parsed = urlparse(self.path)
        if parsed.path == "/":
            self.path = "/frontend/index.html"
            return super().do_GET()
        if parsed.path.startswith("/css/") or parsed.path.startswith("/js/"):
            self.path = "/frontend" + parsed.path
            return super().do_GET()

        if parsed.path == "/api/patients":
            query = parse_qs(parsed.query).get("search", [""])[0].strip()
            if query:
                if not query.isdigit() or int(query) <= 0:
                    self.send_json({"ok": False, "error": "Search with a positive numeric patient ID."}, 400)
                    return
                data, status = run_c("search", query)
                self.send_json(data, status)
                return
            data, status = run_c("list")
            self.send_json(data, status)
            return

        if parsed.path == "/api/resources":
            data, status = run_c("list")
            if status == 200:
                self.send_json({"ok": True, "resources": data["resources"]}, 200)
            else:
                self.send_json(data, status)
            return

        return super().do_GET()

    def do_POST(self):
        parsed = urlparse(self.path)
        try:
            length = int(self.headers.get("Content-Length", "0"))
            if length <= 0 or length > 100_000:
                self.send_json({"ok": False, "error": "Invalid request body."}, 400)
                return
            payload = json.loads(self.rfile.read(length).decode("utf-8"))
        except (ValueError, UnicodeDecodeError, json.JSONDecodeError):
            self.send_json({"ok": False, "error": "Request must contain valid JSON."}, 400)
            return

        if parsed.path == "/api/patients":
            required = ("id", "name", "age", "severity", "department")
            if not isinstance(payload, dict) or any(key not in payload for key in required):
                self.send_json({"ok": False, "error": "Missing patient fields."}, 400)
                return
            try:
                patient_id = int(payload["id"])
                age = int(payload["age"])
                severity = int(payload["severity"])
            except (TypeError, ValueError):
                self.send_json({"ok": False, "error": "ID, age and severity must be numbers."}, 400)
                return
            name = payload["name"]
            department = payload["department"]
            if not isinstance(name, str) or not isinstance(department, str):
                self.send_json({"ok": False, "error": "Name and department must be text."}, 400)
                return
            if not name.strip() or not department.strip():
                self.send_json({"ok": False, "error": "Name and department are required."}, 400)
                return
            data, status = run_c("register", patient_id, name.strip(), age, severity, department.strip())
            self.send_json(data, 201 if status == 200 else status)
            return

        if parsed.path == "/api/beds":
            if not isinstance(payload, dict):
                self.send_json({"ok": False, "error": "Invalid bed request."}, 400)
                return
            action = payload.get("action")
            try:
                patient_id = int(payload.get("patient_id"))
            except (TypeError, ValueError):
                self.send_json({"ok": False, "error": "Patient ID must be numeric."}, 400)
                return
            if action not in ("assign", "release"):
                self.send_json({"ok": False, "error": "Action must be assign or release."}, 400)
                return
            data, status = run_c(action, patient_id)
            self.send_json(data, status)
            return

        self.send_json({"ok": False, "error": "Unknown endpoint."}, 404)

    def log_message(self, fmt, *args):
        print("%s - %s" % (self.log_date_time_string(), fmt % args))

if __name__ == "__main__":
    compile_backend()
    print("Smart Hospital dashboard: http://127.0.0.1:8000/")
    print("Keep this terminal open while using the dashboard. Press Ctrl+C to stop.")
    ThreadingHTTPServer(("127.0.0.1", 8000), Handler).serve_forever()

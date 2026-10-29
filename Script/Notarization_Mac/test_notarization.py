"""Test notarization gates with fake Apple tools; never submit an artifact."""

import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest


class NotarizationTests(unittest.TestCase):
    def run_script(self, status="Accepted", issues=None, submit_exit=0):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            (root / "Effekseer/Effekseer.app").mkdir(parents=True)
            shutil.copy2(Path(__file__).with_name("Effekseer_cert.sh"), root)
            # Signing policy has separate tests; isolate the orchestration here.
            (root / "sign_app.py").write_text("")
            tools = root / "bin"
            tools.mkdir()
            stub = "#!" + sys.executable + "\n" + '''
import json, os, pathlib, sys
name = pathlib.Path(sys.argv[0]).name
args = sys.argv[1:]
with open(os.environ['TEST_COMMAND_LOG'], 'a') as stream:
    stream.write(json.dumps([name] + args) + '\\n')
if name == 'xcrun' and args[:2] == ['notarytool', 'submit']:
    print(json.dumps({'id': 'test-submission'}))
    sys.exit(int(os.environ['TEST_SUBMIT_EXIT']))
elif name == 'xcrun' and args[:2] == ['notarytool', 'log']:
    pathlib.Path(args[-1]).write_text(os.environ['TEST_NOTARY_LOG'])
elif name == 'ditto':
    pathlib.Path(args[-1]).touch()
elif name == 'hdiutil':
    pathlib.Path(args[1]).touch()
'''
            for name in ("xcrun", "ditto", "hdiutil", "codesign", "spctl"):
                path = tools / name
                path.write_text(stub)
                path.chmod(0o755)
            command_log = root / "commands.jsonl"
            result = subprocess.run(
                ["sh", "Effekseer_cert.sh", "Test identity", "Test profile"],
                cwd=root, capture_output=True, text=True, timeout=30,
                env=dict(os.environ, PATH=str(tools) + os.pathsep + os.environ["PATH"],
                         TEST_COMMAND_LOG=str(command_log), TEST_SUBMIT_EXIT=str(submit_exit),
                         TEST_NOTARY_LOG=json.dumps({"status": status, "issues": issues})))
            calls = [json.loads(line) for line in command_log.read_text().splitlines()]
            return result, calls

    def test_accepted_submissions_are_logged_and_validated(self):
        result, calls = self.run_script()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(sum(call[:3] == ["xcrun", "notarytool", "log"] for call in calls), 2)
        self.assertIn(["xcrun", "stapler", "validate", "Effekseer/Effekseer.app"], calls)
        self.assertIn(["xcrun", "stapler", "validate", "Effekseer.dmg"], calls)
        self.assertTrue(any(call[0] == "spctl" for call in calls))

    def test_warning_stops_packaging(self):
        result, calls = self.run_script(issues=[{"severity": "warning"}])
        self.assertNotEqual(result.returncode, 0)
        self.assertFalse(any(call[:2] == ["xcrun", "stapler"] for call in calls))

    def test_invalid_submission_stops_even_when_command_succeeds(self):
        result, calls = self.run_script(status="Invalid")
        self.assertNotEqual(result.returncode, 0)
        self.assertFalse(any(call[0] == "hdiutil" for call in calls))

    def test_failed_submission_still_downloads_log(self):
        result, calls = self.run_script(status="Invalid", submit_exit=1)
        self.assertNotEqual(result.returncode, 0)
        self.assertTrue(any(call[:3] == ["xcrun", "notarytool", "log"] for call in calls))


if __name__ == "__main__":
    unittest.main()

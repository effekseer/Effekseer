import importlib.util
from pathlib import Path
import plistlib
import subprocess
import tempfile
import unittest
from unittest.mock import patch


spec = importlib.util.spec_from_file_location("sign_app", Path(__file__).with_name("sign_app.py"))
sign_app = importlib.util.module_from_spec(spec)
spec.loader.exec_module(sign_app)


class SigningTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.app = Path(self.temp.name) / "Effekseer.app"
        for relative in (sign_app.DOTNET_HOST, sign_app.LAUNCHER,
                         Path("Contents/Resources/createdump"),
                         Path("Contents/Resources/tools/EffekseerResourceConverter"),
                         Path("Contents/Resources/EffekseerMaterialEditor"),
                         Path("Contents/Resources/native.so")):
            path = self.app / relative
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(b"\xcf\xfa\xed\xfe" + b"test")
        self.script = self.app / "Contents/Resources/script.sh"
        self.script.write_text("#!/bin/sh\nexit 0\n")
        self.script.chmod(0o755)

    def test_only_dotnet_host_receives_entitlements(self):
        with patch.object(sign_app.subprocess, "run") as run, patch.object(sign_app, "verify"):
            sign_app.sign(self.app, "Developer ID")
        calls = [call.args[0] for call in run.call_args_list]
        self.assertEqual(len(calls), 7)
        self.assertEqual(calls[-1][-1], str(self.app))
        entitled = [command[-1] for command in calls if "--entitlements" in command]
        self.assertEqual(entitled, [str(self.app / sign_app.DOTNET_HOST)])
        self.assertNotIn(str(self.script), [command[-1] for command in calls])
        for command in calls:
            self.assertIn("--timestamp", command)
            self.assertIn("runtime", command)

    def test_missing_host_fails_before_signing(self):
        (self.app / sign_app.DOTNET_HOST).unlink()
        with patch.object(sign_app.subprocess, "run") as run:
            with self.assertRaises(RuntimeError):
                sign_app.sign(self.app, "Developer ID")
            run.assert_not_called()

    def test_signing_failure_is_fatal(self):
        with patch.object(sign_app.subprocess, "run", side_effect=subprocess.CalledProcessError(1, "codesign")):
            with self.assertRaises(subprocess.CalledProcessError):
                sign_app.sign(self.app, "Developer ID")

    def test_every_architecture_is_audited(self):
        binary = self.app / sign_app.LAUNCHER
        def run(command, **kwargs):
            entitlements = {}
            if "--arch" in command and command[command.index("--arch") + 1] == "x86_64":
                entitlements = sign_app.DOTNET_ENTITLEMENTS
            return subprocess.CompletedProcess(command, 0, plistlib.dumps(entitlements), b"flags=0x10000(runtime)")
        with patch.object(sign_app.subprocess, "check_output", return_value="arm64 x86_64"), \
                patch.object(sign_app.subprocess, "run", side_effect=run):
            with self.assertRaisesRegex(RuntimeError, "Unexpected entitlements"):
                sign_app.verify(self.app, [binary])

    def test_hardened_runtime_is_required(self):
        result = subprocess.CompletedProcess([], 0, b"", b"flags=0x0(none)")
        with patch.object(sign_app.subprocess, "check_output", return_value="arm64"), \
                patch.object(sign_app.subprocess, "run", return_value=result):
            with self.assertRaisesRegex(RuntimeError, "Hardened Runtime missing"):
                sign_app.verify(self.app, [self.app / sign_app.LAUNCHER])

    def test_adhoc_hardened_signature_can_be_audited(self):
        result = subprocess.CompletedProcess([], 0, b"", b"flags=0x10002(adhoc,runtime)")
        with patch.object(sign_app.subprocess, "check_output", return_value="arm64 x86_64"), \
                patch.object(sign_app.subprocess, "run", return_value=result):
            sign_app.verify(self.app, [self.app / sign_app.LAUNCHER])


if __name__ == "__main__":
    unittest.main()

"""Exercise the POSIX launcher against a child process, without the editor."""

import json
import os
from pathlib import Path
import shutil
import signal
import subprocess
import sys
import tempfile
import unittest


LAUNCHER = None


@unittest.skipUnless(os.name == "posix", "POSIX launcher tests")
class LauncherTests(unittest.TestCase):
    def setUp(self):
        if LAUNCHER is None:
            self.skipTest("Run this script with a launcher executable to test")
        self.temp = tempfile.TemporaryDirectory(prefix="effekseer launcher ")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        if sys.platform == "darwin":
            directory = self.root / "Contents/MacOS"
            child_directory = self.root / "Contents/Resources"
        else:
            directory = self.root / "Tool"
            child_directory = directory / "bin"
        directory.mkdir(parents=True)
        child_directory.mkdir(parents=True)
        self.launcher = directory / "EffekseerLauncher"
        shutil.copy2(LAUNCHER, self.launcher)
        self.child = child_directory / "Effekseer"
        self.child.write_text(
            "#!" + sys.executable + "\n"
            "import json, os, signal, sys\n"
            "with open(os.environ['LAUNCHER_TEST_OUTPUT'], 'w') as f:\n"
            "    json.dump(sys.argv[1:], f)\n"
            "if os.environ.get('LAUNCHER_TEST_SIGNAL'):\n"
            "    os.kill(os.getpid(), signal.SIGTERM)\n"
            "sys.exit(int(os.environ.get('LAUNCHER_TEST_EXIT', '0')))\n")
        self.child.chmod(0o755)
        self.output = self.root / "arguments.json"

    def run_launcher(self, arguments=(), **variables):
        return subprocess.run(
            [str(self.launcher), *arguments], cwd=self.root,
            env=dict(os.environ, LAUNCHER_TEST_OUTPUT=str(self.output), **variables),
            capture_output=True, text=True, timeout=30)

    def test_arguments_are_forwarded_verbatim(self):
        marker = self.root / "shell-executed"
        arguments = ["", "space in path.efkefc", 'a"b', "a'b", "日本語", "a\\b",
                     f"$(touch '{marker}')", f"; touch '{marker}'", "*", "--option"]
        result = self.run_launcher(arguments)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(json.loads(self.output.read_text()), arguments)
        self.assertFalse(marker.exists())

    def test_no_arguments(self):
        self.assertEqual(self.run_launcher().returncode, 0)
        self.assertEqual(json.loads(self.output.read_text()), [])

    def test_child_exit_status(self):
        for code in (1, 42, 255):
            with self.subTest(code=code):
                self.assertEqual(self.run_launcher(LAUNCHER_TEST_EXIT=str(code)).returncode, code)

    def test_child_signal(self):
        self.assertEqual(self.run_launcher(LAUNCHER_TEST_SIGNAL="1").returncode, 128 + signal.SIGTERM)

    def test_missing_child(self):
        self.child.unlink()
        self.assertEqual(self.run_launcher().returncode, 1)

    def test_nonexecutable_child(self):
        self.child.chmod(0o644)
        self.assertEqual(self.run_launcher().returncode, 1)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit("Usage: python3 Script/test_launcher.py /path/to/EffekseerLauncher")
    LAUNCHER = Path(sys.argv.pop(1)).resolve()
    unittest.main()

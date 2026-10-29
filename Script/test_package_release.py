import importlib.util
import hashlib
import os
from pathlib import Path
import stat
import subprocess
import sys
import tempfile
import unittest
import zipfile


spec = importlib.util.spec_from_file_location("package_release", Path(__file__).with_name("package_release.py"))
package_release = importlib.util.module_from_spec(spec)
spec.loader.exec_module(package_release)


class PackagingTests(unittest.TestCase):
    def test_cli_writes_checksums_for_distributed_archives(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            for name in ("Linux", "Mac-arm64"):
                source = root / "dist" / name
                source.mkdir(parents=True)
                (source / "readme.txt").write_text(name)
            output = root / "pub"
            subprocess.run([sys.executable, str(Path(__file__).with_name("package_release.py")),
                            str(root / "dist"), str(output), "--epoch", "1600000000"], check=True)
            lines = (output / "SHA256SUMS").read_text().splitlines()
            self.assertEqual(len(lines), 2)
            for line in lines:
                digest, name = line.split("  ")
                self.assertIn(name, ("Linux.zip", "Mac-arm64.zip"))
                self.assertEqual(digest, hashlib.sha256((output / name).read_bytes()).hexdigest())

    def test_metadata_and_file_order_are_reproducible(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            archives = []
            for index, order in enumerate((['z', 'a'], ['a', 'z'])):
                source = root / str(index) / "Effekseer"
                source.mkdir(parents=True)
                for name in order:
                    path = source / name
                    path.write_bytes(name.encode())
                    path.chmod(0o755 if name == 'a' else 0o644)
                    os.utime(path, (1600000000 + index * 100, 1600000000 + index * 100))
                (source / "empty").mkdir()
                (source / "link").symlink_to("a")
                archive = root / f"{index}.zip"
                package_release.package(source, archive, 1600000000)
                archives.append(archive)
            self.assertEqual(archives[0].read_bytes(), archives[1].read_bytes())
            with zipfile.ZipFile(archives[0]) as archive:
                self.assertEqual(archive.read("Effekseer/a"), b"a")
                self.assertEqual(archive.getinfo("Effekseer/a").external_attr >> 16, stat.S_IFREG | 0o755)
                self.assertEqual(archive.getinfo("Effekseer/z").external_attr >> 16, stat.S_IFREG | 0o644)
                self.assertTrue(stat.S_ISLNK(archive.getinfo("Effekseer/link").external_attr >> 16))
                self.assertEqual(archive.read("Effekseer/link"), b"a")
                self.assertIn("Effekseer/empty/", archive.namelist())


if __name__ == "__main__":
    unittest.main()

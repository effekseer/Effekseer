import csv
from pathlib import Path
import tempfile
import unittest

from PIL import Image
from compose_screenshot_comparisons import compose_screenshot_comparisons


class ComposeScreenshotComparisonsTest(unittest.TestCase):
    def make_manifest(self, root, filename, backend):
        sources = root / 'screenshot_comparison_sources'
        sources.mkdir(exist_ok=True)
        image_path = sources / f'{backend}.png'
        Image.new('RGBA', (8, 8), (30, 60, 90, 255)).save(image_path)
        with (sources / filename).open('w', newline='', encoding='utf-8') as stream:
            writer = csv.writer(stream)
            writer.writerow(('composite', 'order', 'label', 'source'))
            for index in range(6):
                writer.writerow((f'comparison_{backend}.png', index, 'RH', image_path.relative_to(root).as_posix()))

    def test_multiple_backends_ignore_stale_legacy_manifest(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for backend in ('DX12', 'Vulkan'):
                self.make_manifest(root, f'manifest_{backend}.csv', backend)
            (root / 'screenshot_comparison_sources' / 'manifest.csv').write_text('stale', encoding='utf-8')
            results = compose_screenshot_comparisons(root)
            self.assertEqual([p.name for p in results], ['comparison_DX12.png', 'comparison_Vulkan.png'])
            for path in results:
                with Image.open(path) as image:
                    self.assertGreater(image.width, 8)

    def test_legacy_manifest(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.make_manifest(root, 'manifest.csv', 'DX11')
            self.assertEqual([p.name for p in compose_screenshot_comparisons(root)], ['comparison_DX11.png'])

    def test_missing_manifest(self):
        with tempfile.TemporaryDirectory() as directory:
            self.assertEqual(compose_screenshot_comparisons(directory), [])


if __name__ == '__main__':
    unittest.main()

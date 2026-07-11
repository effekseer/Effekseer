import unittest
import os
import filecmp
import glob

from PIL import Image, ImageChops


AVERAGE_DELTA_TOLERANCE = 2.0  # per-pixel average grayscale tolerance (0–255 scale)


class ScreenShotTest(unittest.TestCase):
    def test_screenshots(self):
        generated_file_paths = glob.glob('build/Dev/Cpp/Test/Release/*.png')

        failures = []
        warnings = []

        for path in generated_file_paths:
            name = os.path.basename(path)
            test_data_path = 'TestData/Tests/Windows/' + name

            if os.path.exists(test_data_path):
                is_same = filecmp.cmp(test_data_path, path, shallow=False)
                if not is_same:
                    report, avg_delta = describe_difference(test_data_path, path)
                    if avg_delta > AVERAGE_DELTA_TOLERANCE:
                        failures.append(f'{name} differs (avg delta {avg_delta:.2f}/255 > tolerance {AVERAGE_DELTA_TOLERANCE}):\n{indent(report)}')
                    else:
                        warnings.append(f'{name} has minor differences (avg delta {avg_delta:.2f}/255, within tolerance):\n{indent(report)}')
            else:
                failures.append(f'{test_data_path} is not found.')

        if warnings:
            print('Minor differences within tolerance:\n' + '\n'.join(warnings))
        if failures:
            print('Failures:\n' + '\n'.join(failures))

        self.assertFalse(failures, 'Screenshot mismatches found (see log above).')


def indent(text, padding='    '):
    return '\n'.join(padding + line for line in text.splitlines())


def describe_difference(expected_path, actual_path):
    expected_size = os.path.getsize(expected_path)
    actual_size = os.path.getsize(actual_path)
    size_delta = actual_size - expected_size

    lines = [
        f'Expected: {expected_path}',
        f'Actual  : {actual_path}',
        f'File size: expected {expected_size:,} bytes, actual {actual_size:,} bytes (delta {size_delta:+,})',
    ]

    pixel_lines, avg_delta = analyze_pixels(expected_path, actual_path)
    lines.extend(pixel_lines)

    return '\n'.join(lines), avg_delta


def analyze_pixels(expected_path, actual_path):
    try:
        with Image.open(expected_path) as expected_img, Image.open(actual_path) as actual_img:
            expected = expected_img.convert('RGBA')
            actual = actual_img.convert('RGBA')
    except Exception as ex:
        return [f'Failed to inspect pixels: {ex}'], float('inf')

    lines = []
    if expected.size != actual.size:
        lines.append(
            f'Image size: expected {expected.size[0]}x{expected.size[1]}, '
            f'actual {actual.size[0]}x{actual.size[1]} (resize mismatch)'
        )
        return lines, float('inf')

    diff = ImageChops.difference(expected, actual)
    diff_extrema = diff.getextrema()
    max_channel_delta = max(channel[1] for channel in diff_extrema)

    diff_gray = diff.convert('L')
    histogram = diff_gray.histogram()
    total_pixels = diff_gray.width * diff_gray.height
    identical_pixels = histogram[0]
    changed_pixels = total_pixels - identical_pixels

    total_intensity = sum(value * count for value, count in enumerate(histogram))
    average_delta = total_intensity / float(total_pixels) if total_pixels else 0.0

    percent_changed = (changed_pixels / total_pixels) * 100 if total_pixels else 0
    bbox = diff.getbbox()

    if changed_pixels == 0:
        lines.append('Pixel data identical; PNG metadata differs.')
    else:
        lines.append(
            f'Pixel diff: {changed_pixels:,}/{total_pixels:,} pixels changed ({percent_changed:.4g}%).'
        )
        lines.append(
            f'Max channel delta: {max_channel_delta}/255; average grayscale delta: {average_delta:.2f}/255.'
        )
        if bbox:
            lines.append(
                f'Diff bounding box: left={bbox[0]}, top={bbox[1]}, right={bbox[2]}, bottom={bbox[3]}.'
            )

    return lines, average_delta

if __name__ == '__main__':
    unittest.main()

import unittest
import os
import glob

from PIL import Image, ImageChops
from compose_screenshot_comparisons import compose_screenshot_comparisons


class ScreenShotTest(unittest.TestCase):
    def test_screenshots(self):
        composed_paths = compose_screenshot_comparisons('build/Dev/Cpp/Test/Release')
        for path in composed_paths:
            print(f'Composed screenshot comparison: {path}')

        generated_file_paths = glob.glob('build/Dev/Cpp/Test/Release/*.png')

        failures = []

        for path in generated_file_paths:
            name = os.path.basename(path)
            test_data_path = 'TestData/Tests/Windows/' + name

            if os.path.exists(test_data_path):
                is_same = have_identical_pixels(test_data_path, path)
                if not is_same:
                    report = describe_difference(test_data_path, path)
                    failures.append(f'{name} differs:\n{indent(report)}')
            else:
                failures.append(f'{test_data_path} is not found.')

        if failures:
            print('\n'.join(failures))

        self.assertFalse(failures, 'Screenshot mismatches found (see log above).')


def indent(text, padding='    '):
    return '\n'.join(padding + line for line in text.splitlines())


def have_identical_pixels(expected_path, actual_path):
    try:
        with Image.open(expected_path) as expected_img, Image.open(actual_path) as actual_img:
            expected = expected_img.convert('RGBA')
            actual = actual_img.convert('RGBA')
            if expected.size != actual.size:
                return False
            return difference_bbox(ImageChops.difference(expected, actual)) is None
    except Exception:
        return False


def describe_difference(expected_path, actual_path):
    expected_size = os.path.getsize(expected_path)
    actual_size = os.path.getsize(actual_path)
    size_delta = actual_size - expected_size

    lines = [
        f'Expected: {expected_path}',
        f'Actual  : {actual_path}',
        f'File size: expected {expected_size:,} bytes, actual {actual_size:,} bytes (delta {size_delta:+,})',
    ]

    lines.extend(analyze_pixels(expected_path, actual_path))

    return '\n'.join(lines)


def difference_bbox(diff):
    channel_boxes = []
    for channel in diff.split():
        box = channel.getbbox()
        if box is not None:
            channel_boxes.append(box)
    if not channel_boxes:
        return None
    return (
        min(box[0] for box in channel_boxes),
        min(box[1] for box in channel_boxes),
        max(box[2] for box in channel_boxes),
        max(box[3] for box in channel_boxes),
    )


def analyze_pixels(expected_path, actual_path):
    try:
        with Image.open(expected_path) as expected_img, Image.open(actual_path) as actual_img:
            expected = expected_img.convert('RGBA')
            actual = actual_img.convert('RGBA')
    except Exception as ex:
        return [f'Failed to inspect pixels: {ex}']

    lines = []
    if expected.size != actual.size:
        lines.append(
            f'Image size: expected {expected.size[0]}x{expected.size[1]}, '
            f'actual {actual.size[0]}x{actual.size[1]} (resize mismatch)'
        )
        return lines

    diff = ImageChops.difference(expected, actual)
    diff_channels = diff.split()
    diff_mask = diff_channels[0]
    for channel in diff_channels[1:]:
        diff_mask = ImageChops.lighter(diff_mask, channel)

    histogram = diff_mask.histogram()
    total_pixels = diff_mask.width * diff_mask.height
    identical_pixels = histogram[0]
    changed_pixels = total_pixels - identical_pixels

    total_intensity = sum(value * count for value, count in enumerate(histogram))
    average_delta = total_intensity / float(total_pixels) if total_pixels else 0.0
    max_channel_delta = diff_mask.getextrema()[1]

    percent_changed = (changed_pixels / total_pixels) * 100 if total_pixels else 0
    bbox = difference_bbox(diff)

    if changed_pixels == 0:
        lines.append('Pixel data identical; PNG metadata differs.')
    else:
        lines.append(
            f'Pixel diff: {changed_pixels:,}/{total_pixels:,} pixels changed ({percent_changed:.4g}%).'
        )
        lines.append(
            f'Max channel delta: {max_channel_delta}/255; average max-channel delta: {average_delta:.2f}/255.'
        )
        if bbox:
            lines.append(
                f'Diff bounding box: left={bbox[0]}, top={bbox[1]}, right={bbox[2]}, bottom={bbox[3]}.'
            )

    return lines

if __name__ == '__main__':
    unittest.main()

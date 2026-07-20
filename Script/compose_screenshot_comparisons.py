import argparse
import csv
from collections import defaultdict
from pathlib import Path

from PIL import Image, ImageDraw


COLUMNS = 3
EXPECTED_PANEL_COUNT = 6
GAP = 2
HEADER_HEIGHT = 22
FONT_SCALE = 2

FONT_5X7 = {
    'A': ('01110', '10001', '10001', '11111', '10001', '10001', '10001'),
    'C': ('01111', '10000', '10000', '10000', '10000', '10000', '01111'),
    'D': ('11110', '10001', '10001', '10001', '10001', '10001', '11110'),
    'E': ('11111', '10000', '10000', '11110', '10000', '10000', '11111'),
    'F': ('11111', '10000', '10000', '11110', '10000', '10000', '10000'),
    'G': ('01111', '10000', '10000', '10111', '10001', '10001', '01111'),
    'H': ('10001', '10001', '10001', '11111', '10001', '10001', '10001'),
    'I': ('11111', '00100', '00100', '00100', '00100', '00100', '11111'),
    'L': ('10000', '10000', '10000', '10000', '10000', '10000', '11111'),
    'N': ('10001', '11001', '10101', '10011', '10001', '10001', '10001'),
    'P': ('11110', '10001', '10001', '11110', '10000', '10000', '10000'),
    'R': ('11110', '10001', '10001', '11110', '10100', '10010', '10001'),
    'T': ('11111', '00100', '00100', '00100', '00100', '00100', '00100'),
    'U': ('10001', '10001', '10001', '10001', '10001', '10001', '01110'),
    'X': ('10001', '10001', '01010', '00100', '01010', '10001', '10001'),
    'Y': ('10001', '10001', '01010', '00100', '00100', '00100', '00100'),
    'Z': ('11111', '00001', '00010', '00100', '01000', '10000', '11111'),
    '+': ('00000', '00100', '00100', '11111', '00100', '00100', '00000'),
    '-': ('00000', '00000', '00000', '11111', '00000', '00000', '00000'),
}


def draw_label(draw, label, left, top, width):
    glyph_width = 5 * FONT_SCALE
    glyph_gap = FONT_SCALE
    text_width = len(label) * glyph_width + max(0, len(label) - 1) * glyph_gap
    x = left + (width - text_width) // 2
    y = top + (HEADER_HEIGHT - 7 * FONT_SCALE) // 2

    for character in label:
        glyph = FONT_5X7.get(character)
        if glyph is None:
            raise ValueError(f'Unsupported comparison label character: {character!r}')
        for row, bits in enumerate(glyph):
            for column, bit in enumerate(bits):
                if bit == '1':
                    pixel_left = x + column * FONT_SCALE
                    pixel_top = y + row * FONT_SCALE
                    draw.rectangle(
                        (pixel_left, pixel_top, pixel_left + FONT_SCALE - 1, pixel_top + FONT_SCALE - 1),
                        fill=(240, 240, 240, 255),
                    )
        x += glyph_width + glyph_gap


def load_manifest(manifest_path):
    groups = defaultdict(list)
    with manifest_path.open(newline='', encoding='utf-8') as manifest:
        reader = csv.DictReader(manifest)
        required_columns = {'composite', 'order', 'label', 'source'}
        if set(reader.fieldnames or []) != required_columns:
            raise ValueError(
                f'{manifest_path}: expected columns {sorted(required_columns)}, got {reader.fieldnames}'
            )

        for row in reader:
            groups[row['composite']].append(
                (int(row['order']), row['label'], row['source'])
            )
    return groups


def compose_group(output_directory, composite_name, panels):
    if Path(composite_name).name != composite_name:
        raise ValueError(f'Composite name must be a file name: {composite_name}')
    if len(panels) != EXPECTED_PANEL_COUNT:
        raise ValueError(
            f'{composite_name}: expected {EXPECTED_PANEL_COUNT} panels, got {len(panels)}'
        )

    panels = sorted(panels)
    orders = [panel[0] for panel in panels]
    if orders != list(range(EXPECTED_PANEL_COUNT)):
        raise ValueError(f'{composite_name}: panel order must be 0..5, got {orders}')

    loaded_panels = []
    try:
        for _, label, source in panels:
            source_path = output_directory / Path(source)
            image = Image.open(source_path).convert('RGBA')
            loaded_panels.append((label, image))

        panel_size = loaded_panels[0][1].size
        if any(image.size != panel_size for _, image in loaded_panels):
            sizes = [image.size for _, image in loaded_panels]
            raise ValueError(f'{composite_name}: source image sizes differ: {sizes}')

        panel_width, panel_height = panel_size
        rows = (EXPECTED_PANEL_COUNT + COLUMNS - 1) // COLUMNS
        cell_height = HEADER_HEIGHT + panel_height
        canvas_size = (
            COLUMNS * panel_width + (COLUMNS + 1) * GAP,
            rows * cell_height + (rows + 1) * GAP,
        )
        canvas = Image.new('RGBA', canvas_size, (34, 38, 44, 255))
        draw = ImageDraw.Draw(canvas)

        for panel_index, (label, image) in enumerate(loaded_panels):
            column = panel_index % COLUMNS
            row = panel_index // COLUMNS
            left = GAP + column * (panel_width + GAP)
            top = GAP + row * (cell_height + GAP)
            draw.rectangle(
                (left, top, left + panel_width - 1, top + HEADER_HEIGHT - 1),
                fill=(20, 23, 27, 255),
            )
            draw_label(draw, label, left, top, panel_width)
            canvas.alpha_composite(image, (left, top + HEADER_HEIGHT))

        destination = output_directory / composite_name
        canvas.save(destination, format='PNG', compress_level=9)
        return destination
    finally:
        for _, image in loaded_panels:
            image.close()


def compose_screenshot_comparisons(output_directory):
    output_directory = Path(output_directory)
    manifest_path = output_directory / 'screenshot_comparison_sources' / 'manifest.csv'
    if not manifest_path.exists():
        return []

    groups = load_manifest(manifest_path)
    return [
        compose_group(output_directory, composite_name, panels)
        for composite_name, panels in sorted(groups.items())
    ]


def main():
    parser = argparse.ArgumentParser(
        description='Compose labeled screenshot comparison grids from raw test captures.'
    )
    parser.add_argument(
        'output_directory',
        nargs='?',
        default='build/Dev/Cpp/Test/Release',
        help='Directory containing screenshot_comparison_sources/manifest.csv',
    )
    args = parser.parse_args()

    destinations = compose_screenshot_comparisons(args.output_directory)
    for destination in destinations:
        print(f'Composed screenshot comparison: {destination}')


if __name__ == '__main__':
    main()

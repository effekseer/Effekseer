import argparse
import csv
import re
from collections import defaultdict
from itertools import combinations
from pathlib import Path

try:
    from PIL import Image, ImageChops, ImageDraw, ImageFont, ImageOps
except ImportError as ex:
    raise SystemExit("Pillow is required. Install it with: python -m pip install pillow") from ex


DEFAULT_BACKENDS = ["DX11", "Vulkan", "DX12", "GL", "DX9", "Metal", "WebGPU"]


def parse_args():
    parser = argparse.ArgumentParser(
        description="Generate visual and CSV comparisons from screenshot test PNGs."
    )
    parser.add_argument(
        "input_dir",
        nargs="?",
        default="build/Dev/Cpp/Test/Release",
        help="Directory containing screenshot test PNGs.",
    )
    parser.add_argument(
        "--output-dir",
        default=None,
        help="Directory for comparison output. Defaults to <input_dir>/comparison_visuals.",
    )
    parser.add_argument(
        "--backends",
        default=",".join(DEFAULT_BACKENDS),
        help="Comma-separated backend suffixes to group, in display order.",
    )
    parser.add_argument(
        "--reference-backend",
        default="DX11",
        help="Backend used for per-case heatmaps when present.",
    )
    parser.add_argument(
        "--baseline-dir",
        default=None,
        help="Optional reference screenshot directory for baseline_report.csv.",
    )
    parser.add_argument(
        "--top",
        type=int,
        default=20,
        help="Number of rows in top_backend_differences.png.",
    )
    parser.add_argument(
        "--pattern",
        default="*.png",
        help="Glob pattern for screenshot files.",
    )
    parser.add_argument(
        "--diff-boost",
        type=int,
        default=8,
        help="Multiplier for diff heatmap visibility.",
    )
    return parser.parse_args()


def safe_name(value):
    return re.sub(r"[^A-Za-z0-9_.-]+", "_", value)


def load_fonts():
    try:
        return (
            ImageFont.truetype("arial.ttf", 14),
            ImageFont.truetype("arial.ttf", 12),
            ImageFont.truetype("arialbd.ttf", 14),
        )
    except Exception:
        font = ImageFont.load_default()
        return font, font, font


def collect_images(input_dir, pattern, backends):
    images = defaultdict(dict)
    for path in sorted(input_dir.glob(pattern)):
        if not path.is_file():
            continue
        stem = path.stem
        for backend in backends:
            suffix = "_" + backend
            if stem.endswith(suffix):
                images[stem[: -len(suffix)]][backend] = path
                break
    return dict(images)


def fit_image(path, size, background):
    with Image.open(path) as image:
        image = image.convert("RGBA")
        image.thumbnail(size, Image.Resampling.LANCZOS)
        canvas = Image.new("RGBA", size, background)
        canvas.alpha_composite(image, ((size[0] - image.width) // 2, (size[1] - image.height) // 2))
        return canvas


def diff_stats(path_a, path_b):
    with Image.open(path_a) as image_a, Image.open(path_b) as image_b:
        a = image_a.convert("RGBA")
        b = image_b.convert("RGBA")
        if a.size != b.size:
            return {
                "same_size": False,
                "width": "",
                "height": "",
                "pixels": "",
                "changed_pixels": "",
                "changed_percent": "",
                "max_channel_delta": "",
                "average_luma_delta": "",
                "diff_image": None,
            }

        diff = ImageChops.difference(a, b)
        gray = diff.convert("L")
        histogram = gray.histogram()
        pixels = a.size[0] * a.size[1]
        changed_pixels = pixels - histogram[0]
        changed_percent = changed_pixels * 100.0 / pixels if pixels else 0.0
        max_channel_delta = max(channel[1] for channel in diff.getextrema())
        average_luma_delta = (
            sum(value * count for value, count in enumerate(histogram)) / pixels if pixels else 0.0
        )

        return {
            "same_size": True,
            "width": a.size[0],
            "height": a.size[1],
            "pixels": pixels,
            "changed_pixels": changed_pixels,
            "changed_percent": changed_percent,
            "max_channel_delta": max_channel_delta,
            "average_luma_delta": average_luma_delta,
            "diff_image": diff,
        }


def write_diff_report(output_dir, images, backends):
    rows = []
    for case, by_backend in sorted(images.items()):
        present = [backend for backend in backends if backend in by_backend]
        for backend_a, backend_b in combinations(present, 2):
            stats = diff_stats(by_backend[backend_a], by_backend[backend_b])
            rows.append(
                {
                    "case": case,
                    "pair": f"{backend_a}-{backend_b}",
                    "a": str(by_backend[backend_a]),
                    "b": str(by_backend[backend_b]),
                    "same_size": str(stats["same_size"]).lower(),
                    "width": stats["width"],
                    "height": stats["height"],
                    "pixels": stats["pixels"],
                    "changed_pixels": stats["changed_pixels"],
                    "changed_percent": (
                        f"{stats['changed_percent']:.6f}" if stats["changed_percent"] != "" else ""
                    ),
                    "max_channel_delta": stats["max_channel_delta"],
                    "average_luma_delta": (
                        f"{stats['average_luma_delta']:.6f}" if stats["average_luma_delta"] != "" else ""
                    ),
                    "identical": str(stats["changed_pixels"] == 0).lower()
                    if stats["changed_pixels"] != ""
                    else "false",
                }
            )

    path = output_dir / "diff_report.csv"
    if rows:
        with path.open("w", newline="", encoding="utf-8") as file:
            writer = csv.DictWriter(file, fieldnames=list(rows[0].keys()))
            writer.writeheader()
            writer.writerows(rows)
    return path, rows


def write_baseline_report(output_dir, images, baseline_dir):
    rows = []
    for by_backend in images.values():
        for path in sorted(by_backend.values()):
            expected = baseline_dir / path.name
            if not expected.exists():
                rows.append(
                    {
                        "name": path.name,
                        "baseline_exists": "false",
                        "same_size": "",
                        "changed_pixels": "",
                        "changed_percent": "",
                        "max_channel_delta": "",
                        "average_luma_delta": "",
                    }
                )
                continue

            stats = diff_stats(expected, path)
            rows.append(
                {
                    "name": path.name,
                    "baseline_exists": "true",
                    "same_size": str(stats["same_size"]).lower(),
                    "changed_pixels": stats["changed_pixels"],
                    "changed_percent": (
                        f"{stats['changed_percent']:.6f}" if stats["changed_percent"] != "" else ""
                    ),
                    "max_channel_delta": stats["max_channel_delta"],
                    "average_luma_delta": (
                        f"{stats['average_luma_delta']:.6f}" if stats["average_luma_delta"] != "" else ""
                    ),
                }
            )

    path = output_dir / "baseline_report.csv"
    with path.open("w", newline="", encoding="utf-8") as file:
        writer = csv.DictWriter(file, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)
    return path, rows


def parse_float(value):
    if value == "":
        return None
    return float(value)


def write_pair_summary_report(output_dir, diff_rows):
    grouped = defaultdict(list)
    for row in diff_rows:
        grouped[row["pair"]].append(row)

    summary_rows = []
    for pair, rows in sorted(grouped.items()):
        percent_rows = [row for row in rows if row["changed_percent"] != ""]
        percent_values = [parse_float(row["changed_percent"]) for row in percent_rows]
        luma_values = [parse_float(row["average_luma_delta"]) for row in rows if row["average_luma_delta"] != ""]
        max_row = max(percent_rows, key=lambda row: parse_float(row["changed_percent"])) if percent_rows else None
        max_channel_values = [
            int(row["max_channel_delta"]) for row in rows if str(row["max_channel_delta"]).isdigit()
        ]

        summary_rows.append(
            {
                "pair": pair,
                "cases": len(rows),
                "identical": sum(1 for row in rows if row["identical"] == "true"),
                "different": sum(1 for row in rows if row["identical"] != "true"),
                "avg_changed_percent": f"{sum(percent_values) / len(percent_values):.6f}"
                if percent_values
                else "",
                "max_changed_percent": max_row["changed_percent"] if max_row else "",
                "max_case": max_row["case"] if max_row else "",
                "avg_average_luma_delta": f"{sum(luma_values) / len(luma_values):.6f}" if luma_values else "",
                "max_channel_delta": max(max_channel_values) if max_channel_values else "",
            }
        )

    path = output_dir / "backend_pair_summary.csv"
    if summary_rows:
        with path.open("w", newline="", encoding="utf-8") as file:
            writer = csv.DictWriter(file, fieldnames=list(summary_rows[0].keys()))
            writer.writeheader()
            writer.writerows(summary_rows)
    return path, summary_rows


def diff_heatmap(path_a, path_b, size, colors, diff_boost):
    stats = diff_stats(path_a, path_b)
    if not stats["same_size"]:
        canvas = Image.new("RGBA", size, colors["missing"])
        draw = ImageDraw.Draw(canvas)
        draw.text((8, 8), "size mismatch", fill=colors["red"], font=colors["font_small"])
        return canvas, stats

    gray = stats["diff_image"].convert("L")
    gray = gray.point(lambda value: min(255, value * diff_boost))
    heatmap = ImageOps.colorize(gray, black=(255, 255, 255), white=(230, 0, 50)).convert("RGBA")
    heatmap.thumbnail(size, Image.Resampling.LANCZOS)
    canvas = Image.new("RGBA", size, colors["white"])
    canvas.alpha_composite(heatmap, ((size[0] - heatmap.width) // 2, (size[1] - heatmap.height) // 2))
    return canvas, stats


def draw_text_clipped(draw, xy, value, fill, font, max_width):
    value = str(value)
    if draw.textlength(value, font=font) <= max_width:
        draw.text(xy, value, fill=fill, font=font)
        return

    suffix = "..."
    while value and draw.textlength(value + suffix, font=font) > max_width:
        value = value[:-1]
    draw.text(xy, value + suffix, fill=fill, font=font)


def rank_cases(images, backends):
    ranked = []
    for case, by_backend in sorted(images.items()):
        present = [backend for backend in backends if backend in by_backend]
        max_percent = 0.0
        max_pair = ""
        for backend_a, backend_b in combinations(present, 2):
            stats = diff_stats(by_backend[backend_a], by_backend[backend_b])
            if stats["changed_percent"] == "":
                continue
            if stats["changed_percent"] > max_percent:
                max_percent = stats["changed_percent"]
                max_pair = f"{backend_a}-{backend_b}"
        ranked.append((max_percent, max_pair, case))
    ranked.sort(reverse=True)
    return ranked


def make_colors():
    font, font_small, font_bold = load_fonts()
    return {
        "bg": (245, 246, 248, 255),
        "white": (255, 255, 255, 255),
        "line": (210, 214, 220, 255),
        "text": (30, 34, 40, 255),
        "muted": (95, 102, 112, 255),
        "red": (235, 80, 80, 255),
        "missing": (255, 245, 245, 255),
        "font": font,
        "font_small": font_small,
        "font_bold": font_bold,
    }


def blend_color(start, end, amount):
    amount = max(0.0, min(1.0, amount))
    return tuple(int(start[index] + (end[index] - start[index]) * amount) for index in range(4))


def write_backend_pair_summary_sheet(output_dir, summary_rows, backends, colors):
    summary_by_pair = {row["pair"]: row for row in summary_rows}
    cell_w = 188
    cell_h = 94
    row_label_w = 104
    title_h = 74
    pad = 10
    width = row_label_w + cell_w * len(backends) + pad
    height = title_h + cell_h * len(backends) + pad
    sheet = Image.new("RGBA", (width, height), colors["bg"])
    draw = ImageDraw.Draw(sheet)
    draw.text((pad, pad), "Backend pair summary", fill=colors["text"], font=colors["font_bold"])
    draw.text(
        (pad, pad + 20),
        "avg changed %, max changed %, and identical cases",
        fill=colors["muted"],
        font=colors["font_small"],
    )

    for col, backend in enumerate(backends):
        x = row_label_w + col * cell_w
        draw_text_clipped(draw, (x + 8, title_h - 27), backend, colors["text"], colors["font_bold"], cell_w - 16)

    for row_index, backend_y in enumerate(backends):
        y = title_h + row_index * cell_h
        draw_text_clipped(
            draw,
            (pad, y + 36),
            backend_y,
            colors["text"],
            colors["font_bold"],
            row_label_w - 2 * pad,
        )
        for col_index, backend_x in enumerate(backends):
            x = row_label_w + col_index * cell_w
            rect = (x, y, x + cell_w - 1, y + cell_h - 1)
            if backend_x == backend_y:
                draw.rectangle(rect, fill=colors["white"], outline=colors["line"])
                draw_text_clipped(
                    draw,
                    (x + 8, y + 36),
                    backend_x,
                    colors["muted"],
                    colors["font"],
                    cell_w - 16,
                )
                continue

            pair = (
                f"{backend_x}-{backend_y}"
                if backends.index(backend_x) < backends.index(backend_y)
                else f"{backend_y}-{backend_x}"
            )
            summary = summary_by_pair.get(pair)
            if not summary:
                draw.rectangle(rect, fill=colors["missing"], outline=colors["line"])
                draw.text((x + 8, y + 36), "missing", fill=colors["muted"], font=colors["font_small"])
                continue

            avg_percent = parse_float(summary["avg_changed_percent"]) or 0.0
            fill = blend_color(colors["white"], (255, 210, 210, 255), min(avg_percent / 15.0, 1.0))
            draw.rectangle(rect, fill=fill, outline=colors["line"])
            draw_text_clipped(draw, (x + 8, y + 8), pair, colors["text"], colors["font_bold"], cell_w - 16)
            draw.text(
                (x + 8, y + 30),
                f"avg {avg_percent:.3f}%",
                fill=colors["text"],
                font=colors["font_small"],
            )
            draw.text(
                (x + 8, y + 48),
                f"max {summary['max_changed_percent']}%",
                fill=colors["text"],
                font=colors["font_small"],
            )
            draw.text(
                (x + 8, y + 66),
                f"same {summary['identical']}/{summary['cases']}",
                fill=colors["muted"],
                font=colors["font_small"],
            )

    path = output_dir / "backend_pair_summary.png"
    sheet.convert("RGB").save(path, quality=95)
    return path


def write_backend_pair_sheets(output_dir, diff_rows, colors, diff_boost):
    pair_dir = output_dir / "pairs"
    pair_dir.mkdir(exist_ok=True)
    grouped = defaultdict(list)
    for row in diff_rows:
        grouped[row["pair"]].append(row)

    thumb_size = (128, 128)
    pad = 10
    label_h = 22
    case_label_w = 230
    metric_w = 180
    row_h = label_h + thumb_size[1] + pad
    width = case_label_w + (thumb_size[0] + pad) * 3 + metric_w + pad

    for pair, rows in sorted(grouped.items()):
        sorted_rows = sorted(
            rows,
            key=lambda row: parse_float(row["changed_percent"]) if row["changed_percent"] != "" else 101.0,
            reverse=True,
        )
        height = 58 + row_h * len(sorted_rows) + pad
        sheet = Image.new("RGBA", (width, height), colors["bg"])
        draw = ImageDraw.Draw(sheet)
        draw.text((pad, pad), f"{pair} backend comparison", fill=colors["text"], font=colors["font_bold"])
        draw.text(
            (pad, pad + 20),
            "Rows are sorted by changed pixel percentage.",
            fill=colors["muted"],
            font=colors["font_small"],
        )

        backend_a, backend_b = pair.split("-")
        x_a = case_label_w
        x_b = x_a + thumb_size[0] + pad
        x_diff = x_b + thumb_size[0] + pad
        x_metric = x_diff + thumb_size[0] + pad
        draw.text((x_a, 36), backend_a, fill=colors["text"], font=colors["font_bold"])
        draw.text((x_b, 36), backend_b, fill=colors["text"], font=colors["font_bold"])
        draw.text((x_diff, 36), "diff", fill=colors["text"], font=colors["font_bold"])
        draw.text((x_metric, 36), "metrics", fill=colors["text"], font=colors["font_bold"])

        for index, row in enumerate(sorted_rows):
            y = 58 + index * row_h
            if index % 2 == 0:
                draw.rectangle((0, y - 2, width, y + row_h - 2), fill=(250, 251, 252, 255))

            draw_text_clipped(
                draw,
                (pad, y + label_h + 42),
                row["case"],
                colors["text"],
                colors["font_bold"],
                case_label_w - 2 * pad,
            )

            path_a = Path(row["a"])
            path_b = Path(row["b"])
            sheet.alpha_composite(fit_image(path_a, thumb_size, colors["white"]), (x_a, y + label_h))
            sheet.alpha_composite(fit_image(path_b, thumb_size, colors["white"]), (x_b, y + label_h))
            heatmap, _ = diff_heatmap(path_a, path_b, thumb_size, colors, diff_boost)
            sheet.alpha_composite(heatmap, (x_diff, y + label_h))

            for x in (x_a, x_b, x_diff):
                draw.rectangle(
                    (x, y + label_h, x + thumb_size[0], y + label_h + thumb_size[1]),
                    outline=colors["line"],
                )

            if row["changed_percent"] == "":
                draw.text((x_metric, y + label_h + 42), "size mismatch", fill=colors["red"], font=colors["font_small"])
            else:
                draw.text(
                    (x_metric, y + label_h + 30),
                    f"{row['changed_percent']}%",
                    fill=colors["text"],
                    font=colors["font_bold"],
                )
                draw.text(
                    (x_metric, y + label_h + 50),
                    f"changed {row['changed_pixels']}",
                    fill=colors["muted"],
                    font=colors["font_small"],
                )
                draw.text(
                    (x_metric, y + label_h + 68),
                    f"max {row['max_channel_delta']}, luma {row['average_luma_delta']}",
                    fill=colors["muted"],
                    font=colors["font_small"],
                )

        sheet.convert("RGB").save(pair_dir / f"{safe_name(pair.replace('-', '_vs_'))}.png", quality=95)

    return pair_dir


def write_case_sheets(output_dir, images, ranked_cases, backends, reference_backend, colors, diff_boost):
    case_dir = output_dir / "cases"
    case_dir.mkdir(exist_ok=True)

    thumb_size = (144, 144)
    pad = 10
    label_h = 22
    case_label_w = 210
    metric_h = 28
    reference = reference_backend

    for _, _, case in ranked_cases:
        by_backend = images[case]
        present = [backend for backend in backends if backend in by_backend]
        compare_backends = [backend for backend in present if backend != reference]

        width = case_label_w + max(len(present), len(compare_backends), 1) * (thumb_size[0] + pad) + pad
        height = 36 + label_h + thumb_size[1] + pad + label_h + thumb_size[1] + metric_h + pad
        sheet = Image.new("RGBA", (width, height), colors["bg"])
        draw = ImageDraw.Draw(sheet)
        draw.text((pad, pad), case, fill=colors["text"], font=colors["font_bold"])

        y = 36
        x = case_label_w
        for backend in present:
            draw.text((x, y), backend, fill=colors["text"], font=colors["font_bold"])
            sheet.alpha_composite(fit_image(by_backend[backend], thumb_size, colors["white"]), (x, y + label_h))
            draw.rectangle(
                (x, y + label_h, x + thumb_size[0], y + label_h + thumb_size[1]),
                outline=colors["line"],
            )
            x += thumb_size[0] + pad

        y = 36 + label_h + thumb_size[1] + pad
        if reference not in by_backend:
            draw.text((pad, y + label_h + 44), f"{reference} image missing", fill=colors["muted"], font=colors["font"])
        else:
            draw.text(
                (pad, y + label_h + 44),
                f"Diff heatmap vs {reference}",
                fill=colors["muted"],
                font=colors["font"],
            )
            x = case_label_w
            for backend in compare_backends:
                draw.text((x, y), f"{reference} vs {backend}", fill=colors["text"], font=colors["font_bold"])
                heatmap, stats = diff_heatmap(
                    by_backend[reference], by_backend[backend], thumb_size, colors, diff_boost
                )
                sheet.alpha_composite(heatmap, (x, y + label_h))
                draw.rectangle(
                    (x, y + label_h, x + thumb_size[0], y + label_h + thumb_size[1]),
                    outline=colors["line"],
                )
                if stats["changed_percent"] != "":
                    draw.text(
                        (x, y + label_h + thumb_size[1] + 3),
                        f"{stats['changed_percent']:.3f}%  max {stats['max_channel_delta']}",
                        fill=colors["muted"],
                        font=colors["font_small"],
                    )
                x += thumb_size[0] + pad

        sheet.convert("RGB").save(case_dir / f"{safe_name(case)}.png", quality=95)

    return case_dir


def write_top_sheet(output_dir, images, ranked_cases, backends, colors, diff_boost, top_count):
    thumb_size = (144, 144)
    pad = 10
    label_h = 22
    case_label_w = 210
    metric_h = 28
    top_cases = ranked_cases[:top_count]
    row_h = label_h + thumb_size[1] + metric_h + pad
    width = case_label_w + (len(backends) + 1) * (thumb_size[0] + pad) + pad
    height = 42 + row_h * len(top_cases) + pad
    sheet = Image.new("RGBA", (width, height), colors["bg"])
    draw = ImageDraw.Draw(sheet)
    draw.text((pad, pad), "Top backend screenshot differences", fill=colors["text"], font=colors["font_bold"])

    for index, (max_percent, max_pair, case) in enumerate(top_cases):
        y = 42 + index * row_h
        draw_text_clipped(
            draw,
            (pad, y + label_h + 45),
            case,
            colors["text"],
            colors["font_bold"],
            case_label_w - 2 * pad,
        )
        draw.text(
            (pad, y + label_h + 64),
            f"max {max_pair}: {max_percent:.3f}%" if max_pair else "single backend",
            fill=colors["muted"],
            font=colors["font_small"],
        )

        x = case_label_w
        by_backend = images[case]
        present = [backend for backend in backends if backend in by_backend]
        for backend in present:
            draw.text((x, y), backend, fill=colors["text"], font=colors["font_bold"])
            sheet.alpha_composite(fit_image(by_backend[backend], thumb_size, colors["white"]), (x, y + label_h))
            draw.rectangle(
                (x, y + label_h, x + thumb_size[0], y + label_h + thumb_size[1]),
                outline=colors["line"],
            )
            x += thumb_size[0] + pad

        if max_pair:
            backend_a, backend_b = max_pair.split("-")
            draw.text((x, y), f"{max_pair} diff", fill=colors["text"], font=colors["font_bold"])
            heatmap, _ = diff_heatmap(by_backend[backend_a], by_backend[backend_b], thumb_size, colors, diff_boost)
            sheet.alpha_composite(heatmap, (x, y + label_h))
            draw.rectangle(
                (x, y + label_h, x + thumb_size[0], y + label_h + thumb_size[1]),
                outline=colors["line"],
            )

    path = output_dir / "top_backend_differences.png"
    sheet.convert("RGB").save(path, quality=95)
    return path


def write_all_sheet(output_dir, images, ranked_cases, backends, reference_backend, colors, diff_boost):
    small_size = (96, 96)
    pad = 10
    label_h = 22
    case_label_w = 210
    row_h = label_h + small_size[1] + 6
    compare_backends = [backend for backend in backends if backend != reference_backend]
    width = case_label_w + (len(backends) + len(compare_backends)) * (small_size[0] + pad) + pad
    height = 42 + row_h * len(ranked_cases) + pad
    sheet = Image.new("RGBA", (width, height), colors["bg"])
    draw = ImageDraw.Draw(sheet)
    draw.text(
        (pad, pad),
        f"All backend screenshots and {reference_backend}-based diffs",
        fill=colors["text"],
        font=colors["font_bold"],
    )

    for index, (_, _, case) in enumerate(ranked_cases):
        y = 42 + index * row_h
        draw_text_clipped(
            draw,
            (pad, y + label_h + 30),
            case,
            colors["text"],
            colors["font_small"],
            case_label_w - 2 * pad,
        )

        x = case_label_w
        by_backend = images[case]
        for backend in backends:
            if backend not in by_backend:
                continue
            draw.text((x, y), backend, fill=colors["text"], font=colors["font_small"])
            sheet.alpha_composite(fit_image(by_backend[backend], small_size, colors["white"]), (x, y + label_h))
            draw.rectangle(
                (x, y + label_h, x + small_size[0], y + label_h + small_size[1]),
                outline=colors["line"],
            )
            x += small_size[0] + pad

        if reference_backend in by_backend:
            for backend in compare_backends:
                if backend not in by_backend:
                    continue
                draw.text((x, y), f"{reference_backend}-{backend}", fill=colors["text"], font=colors["font_small"])
                heatmap, _ = diff_heatmap(
                    by_backend[reference_backend], by_backend[backend], small_size, colors, diff_boost
                )
                sheet.alpha_composite(heatmap, (x, y + label_h))
                draw.rectangle(
                    (x, y + label_h, x + small_size[0], y + label_h + small_size[1]),
                    outline=colors["line"],
                )
                x += small_size[0] + pad

    path = output_dir / "all_backend_comparisons.png"
    sheet.convert("RGB").save(path, quality=95)
    return path


def print_summary(images, diff_rows, baseline_rows, output_paths):
    complete_case_count = sum(1 for by_backend in images.values() if len(by_backend) > 1)
    identical_pairs = sum(1 for row in diff_rows if row["identical"] == "true")
    different_pairs = len(diff_rows) - identical_pairs
    print(f"cases={len(images)}")
    print(f"cases_with_comparisons={complete_case_count}")
    print(f"images={sum(len(value) for value in images.values())}")
    print(f"comparisons={len(diff_rows)}")
    print(f"identical_pairs={identical_pairs}")
    print(f"different_pairs={different_pairs}")
    if baseline_rows is not None:
        existing = sum(1 for row in baseline_rows if row["baseline_exists"] == "true")
        print(f"baseline_matches_checked={existing}")
        print(f"baseline_missing={len(baseline_rows) - existing}")
    for label, path in output_paths:
        print(f"{label}={path}")


def main():
    args = parse_args()
    input_dir = Path(args.input_dir)
    output_dir = Path(args.output_dir) if args.output_dir else input_dir / "comparison_visuals"
    backends = [backend.strip() for backend in args.backends.split(",") if backend.strip()]

    if not input_dir.exists():
        raise SystemExit(f"Input directory not found: {input_dir}")

    images = collect_images(input_dir, args.pattern, backends)
    if not images:
        raise SystemExit(f"No screenshot images matching backend suffixes were found in: {input_dir}")

    active_backends = [backend for backend in backends if any(backend in value for value in images.values())]
    reference_backend = args.reference_backend if args.reference_backend in active_backends else active_backends[0]

    output_dir.mkdir(parents=True, exist_ok=True)
    diff_report_path, diff_rows = write_diff_report(output_dir, images, active_backends)
    pair_summary_path, pair_summary_rows = write_pair_summary_report(output_dir, diff_rows)

    baseline_rows = None
    baseline_report_path = None
    if args.baseline_dir:
        baseline_report_path, baseline_rows = write_baseline_report(output_dir, images, Path(args.baseline_dir))

    colors = make_colors()
    ranked_cases = rank_cases(images, active_backends)
    pair_summary_sheet = write_backend_pair_summary_sheet(output_dir, pair_summary_rows, active_backends, colors)
    pair_dir = write_backend_pair_sheets(output_dir, diff_rows, colors, args.diff_boost)
    case_dir = write_case_sheets(
        output_dir, images, ranked_cases, active_backends, reference_backend, colors, args.diff_boost
    )
    top_sheet = write_top_sheet(output_dir, images, ranked_cases, active_backends, colors, args.diff_boost, args.top)
    all_sheet = write_all_sheet(
        output_dir, images, ranked_cases, active_backends, reference_backend, colors, args.diff_boost
    )

    outputs = [
        ("diff_report", diff_report_path),
        ("backend_pair_summary", pair_summary_path),
        ("backend_pair_summary_sheet", pair_summary_sheet),
        ("top_sheet", top_sheet),
        ("all_sheet", all_sheet),
        ("pair_sheets", pair_dir),
        ("case_sheets", case_dir),
    ]
    if baseline_report_path:
        outputs.insert(1, ("baseline_report", baseline_report_path))
    print_summary(images, diff_rows, baseline_rows, outputs)


if __name__ == "__main__":
    main()

from typing import List, Dict
from pathlib import Path
import csv
import re
import sys

root_path = Path("./")

lang_keywords: List[str] = ["en", "ja", "es", "zhcn"]

key_to_texts: Dict[str, Dict[str, str]] = {}


def get_csv_pathes(lang_keyword: str) -> List[Path]:
    tte_le = root_path / lang_keyword
    if len(sys.argv) >= 2:
        return [tte_le / v for v in sys.argv[1:]]
    else:
        return list(tte_le.glob("**/*.csv"))


for lang_keyword in lang_keywords:
    tte_le = root_path / lang_keyword
    csv_pathes = get_csv_pathes(lang_keyword)

    for csv_path in csv_pathes:
        relative_path = csv_path.relative_to(tte_le)
        relative_path_key = str(relative_path).replace("\\", "/")

        print(f"Loading {relative_path}")
        with open(csv_path, encoding="utf-8") as f:
            reader = csv.reader(f)
            for row in reader:
                if len(row) == 0 or row[0] == "":
                    continue
                key = f"{relative_path_key}@{row[0]}"
                if not (key in key_to_texts.keys()):
                    key_to_texts[key] = {}
                key_to_texts[key][lang_keyword] = row[1]

with open("result_compare.csv", "w", encoding="utf-8") as f:
    writer = csv.writer(f, lineterminator="\n")
    writer.writerow([""] + lang_keywords)
    for (key, text) in key_to_texts.items():
        row = [key]
        for lang_keyword in lang_keywords:
            if lang_keyword in text.keys():
                row.append(text[lang_keyword])
            else:
                row.append("!!!None!!!")
        writer.writerow(row)

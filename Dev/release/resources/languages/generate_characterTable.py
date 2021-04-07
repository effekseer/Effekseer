import csv
from pathlib import Path

target_dirs = [Path('ja'),Path('en')]

fixed_characters = r'''
English
日本語
簡体中文
简体中文
繁體中文
繁体中文

=== auto generated ===

'''

characters = set()

for target_dir in target_dirs:
    for csv_file_path in target_dir.glob('*.csv'):
        with open(csv_file_path, 'r', encoding='utf8') as csvfile:
            reader = csv.reader(csvfile)
            for rows in reader:
                if(len(rows)) < 2:
                    continue
                for c in rows[1]:
                    characters.add(c)


characterTable = fixed_characters

for c in characters:
    characterTable += c

f = open('characterTable.txt', 'w', encoding='utf8')
f.write(characterTable)
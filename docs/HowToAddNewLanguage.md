
# How to add a new language

For example, if you want to add Simplified Chinese:

Copy the directory ```resources/languages/en``` and rename it to
```resources/languages/zhcn```.

Modify the `CharacterTable` entry in
```resources/languages/zhcn/Base.csv```

```
CharacterTable,chinese
```

You can select from ```default```, ```chinese```, ```korean```, and ```thai```.

If your language doesn't already exist or lacks required characters, add them to `characterTable.txt`.

You can set the fonts by editing `Font_Normal` and `Font_Bold` in `Base.csv`.

You can now edit the texts in ```resources/languages/zhcn/```

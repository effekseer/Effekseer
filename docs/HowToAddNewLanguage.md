
# How to add a new language

For example, if you like to add simplified chinese

Copy ``` resources/languages/en ``` and rename a directory.
In this case you rename it into ``` resources/languages/zhcn ```

Change CharacterTable in ``` resources/languages/zhcn/Base.csv ```

```
CharacterTable,chinese
```

You can select from ```default```, ```chinese```, ```korean```, ```thai```

If your language doesn't exists or letters, add letters which is used to characterTable.txt

You can add and change a font by changing ``` Font_Normal ``` and ``` Font_Bold ``` in ``` Base.csv ```

You now can change texts in ```resources/languages/zhcn/ ```

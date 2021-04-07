
# How to add a new language

Add your language key in ``` resources/languages/languages.txt ```
For example, if you like to add simplified chinese

```
ja
en
zhcn
```


Copy ``` resources/languages/en ``` and rename a directory.
The directory's name is as same as language key.
In this case you rename it into ``` resources/languages/zhcn ```

Add your language name in ``` Base.csv ```

```
Language_en,English
Language_ja,日本語
Language_zhcn,简体中文 <- new language
```

Change CharacterTable in ``` resources/languages/zhcn/Base.csv ```

```
CharacterTable,simplified_chinese
```

You can select from ```default```, ```chinese```, ```simplified_chinese```, ```korean```, ```thai```

If your language doesn't exists, add letters which is used to characterTable.txt

You can add and change a font by changing ''' Font_Normal ''' and ''' Font_Bold ''' in ''' Base.csv '''

You now can change texts in ```resources/languages/zhcn/ ```

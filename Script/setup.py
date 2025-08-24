import shutil
import os
import platform

os.chdir(os.path.dirname(os.path.abspath(__file__)))

shutil.copytree('../ResourceData/tool/resources/fonts', '../Dev/release/resources/fonts', dirs_exist_ok=True)
shutil.copytree("../ResourceData/tool/resources/icons","../Dev/release/resources/icons", dirs_exist_ok=True)

pf = platform.system()
if pf == 'Windows':
    shutil.copyfile("../ResourceData/tool/tools/win/EffekseerResourceConverter.exe", "../Dev/release/tools/EffekseerResourceConverter.exe")

if pf == 'Darwin':
    shutil.copyfile("../ResourceData/tool/tools/mac/EffekseerResourceConverter", "../Dev/release/tools/EffekseerResourceConverter")

if pf == 'Linux':
    shutil.copyfile("../ResourceData/tool/tools/linux/EffekseerResourceConverter", "../Dev/release/tools/EffekseerResourceConverter")

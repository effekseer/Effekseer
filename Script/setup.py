import shutil
import os
import platform

os.chdir(os.path.dirname(os.path.abspath(__file__)))

shutil.copytree('../ResourceData/tool/resources/fonts', '../Dev/release/resources/fonts', dirs_exist_ok=True)
shutil.copytree("../ResourceData/tool/resources/icons","../Dev/release/resources/icons", dirs_exist_ok=True)

pf = platform.system()
if pf == 'Windows':
    shutil.copyfile("../ResourceData/tool/tools/win/fbxToEffekseerModelConverter.exe", "../Dev/release/tools/fbxToEffekseerModelConverter.exe")
    shutil.copyfile("../ResourceData/tool/tools/win/fbxToEffekseerCurveConverter.exe", "../Dev/release/tools/fbxToEffekseerCurveConverter.exe")
    shutil.copyfile("../ResourceData/tool/tools/win/libfbxsdk.dll", "../Dev/release/tools/libfbxsdk.dll")

if pf == 'Darwin':
    shutil.copyfile("../ResourceData/tool/tools/mac/fbxToEffekseerModelConverter", "../Dev/release/tools/fbxToEffekseerModelConverter")
    shutil.copyfile("../ResourceData/tool/tools/mac/fbxToEffekseerCurveConverter", "../Dev/release/tools/fbxToEffekseerCurveConverter")
    shutil.copyfile("../ResourceData/tool/tools/mac/libfbxsdk.dylib", "../Dev/release/tools/libfbxsdk.dylib")

if pf == 'Linux':
    shutil.copyfile("../ResourceData/tool/tools/linux/fbxToEffekseerModelConverter", "../Dev/release/tools/fbxToEffekseerModelConverter")
    shutil.copyfile("../ResourceData/tool/tools/linux/fbxToEffekseerCurveConverter", "../Dev/release/tools/fbxToEffekseerCurveConverter")
    shutil.copyfile("../ResourceData/tool/tools/linux/libfbxsdk.so", "../Dev/release/tools/libfbxsdk.so")

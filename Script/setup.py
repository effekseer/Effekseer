import shutil
import os
import platform

os.chdir(os.path.dirname(os.path.abspath(__file__)))

shutil.rmtree('../Dev/release/resources/fonts', ignore_errors=True)
shutil.copytree('../ResourceData/tool/resources/fonts', '../Dev/release/resources/fonts')

pf = platform.system()
if pf == 'Windows':
    shutil.copyfile("../ResourceData/tool/tools/win/fbxToEffekseerModelConverter.exe", "../Dev/release/tools/fbxToEffekseerModelConverter.exe")

if pf == 'Darwin':
    shutil.copyfile("../ResourceData/tool/tools/mac/fbxToEffekseerModelConverter", "../Dev/release/tools/fbxToEffekseerModelConverter")
    shutil.copyfile("../ResourceData/tool/tools/mac/libfbxsdk.dylib", "../Dev/release/tools/libfbxsdk.dylib")
    shutil.copyfile("../ResourceData/tool/tools/mac/mqoToEffekseerModelConverter", "../Dev/release/tools/mqoToEffekseerModelConverter")

shutil.copyfile("../ResourceData/tool/resources/icons/Copy.png", "../Dev/release/resources/icons/Copy.png")
shutil.copyfile("../ResourceData/tool/resources/icons/Paste.png", "../Dev/release/resources/icons/Paste.png")

shutil.copyfile("../ResourceData/tool/resources/icons/Panel_DynamicParameter.png","../Dev/release/resources/icons/Panel_DynamicParameter.png")
shutil.copyfile("../ResourceData/tool/resources/icons/Material_Icon_Sphere.png","../Dev/release/resources/icons/Material_Icon_Sphere.png")
shutil.copyfile("../ResourceData/tool/resources/icons/Material_Icon_Squre.png","../Dev/release/resources/icons/Material_Icon_Squre.png")
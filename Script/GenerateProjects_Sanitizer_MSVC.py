import glob

projs = glob.glob('../build_sanitizer/**/*.vcxproj', recursive=True)

for p in projs:
    f = open(p, 'r', encoding="utf_8_sig")
    proj_str = f.read()

    proj_str = proj_str.replace('<PlatformToolset>v142</PlatformToolset>','<PlatformToolset>v142</PlatformToolset><EnableASAN>true</EnableASAN>')

    f = open(p, 'w', encoding="utf_8_sig")
    f.write(proj_str)

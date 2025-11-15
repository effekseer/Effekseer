import shutil
import os
import platform

os.chdir(os.path.dirname(os.path.abspath(__file__)))

shutil.copytree('../ResourceData/tool/resources/fonts', '../Dev/release/resources/fonts', dirs_exist_ok=True)
shutil.copytree("../ResourceData/tool/resources/icons","../Dev/release/resources/icons", dirs_exist_ok=True)

pf = platform.system()
if pf == 'Windows':
    pass

if pf == 'Darwin':
    pass

if pf == 'Linux':
    pass

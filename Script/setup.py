import shutil
import os

os.chdir(os.path.dirname(os.path.abspath(__file__)))

shutil.rmtree('../Dev/release/resources/fonts', ignore_errors=True)
shutil.copytree('../ResourceData/tool/resources/fonts', '../Dev/release/resources/fonts')

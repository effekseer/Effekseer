import os
import glob
import subprocess

FLATC = "3rdParty/flatbuffers/bin/flatc.exe"

def convert(path: str):
    for filepath in glob.glob(path, recursive=False):
        subprocess.call([FLATC, "--cpp", "-o", os.path.dirname(filepath), filepath])


convert("Effekseer/Effekseer/Network/data/*.fbs")
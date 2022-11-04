import os
import zipfile
import shutil
import glob

def unzip(zipped_dir, unzipped_dir):

    # make out dir
    if not os.path.exists(unzipped_dir):
        os.mkdir(unzipped_dir)

    # unzip
    files = glob.glob(zipped_dir + "*.zip")
    for file in files:
        shutil.unpack_archive(file, unzipped_dir)
        print("unzipped: "+ file)
    

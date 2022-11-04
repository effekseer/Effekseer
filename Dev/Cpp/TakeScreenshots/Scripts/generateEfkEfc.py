import unzipping
import os
import pathlib
import shutil
import glob
import subprocess

processed_dir = "./processed/"
processed_dir_legacy = "./processed/1.7/"
processed_dir_newer = "./processed/1.8/"

tool_dir_legacy = "./Tool/1.7/"
tool_dir_newer = "./Tool/1.8/"

zipped_dir = "./zipData/"
unzipped_dir = "./unzipped/"

# unzip and copy files 
def prepare_process():
    unzipping.unzip(zipped_dir=zipped_dir, unzipped_dir=unzipped_dir)
    
    if os.path.exists(processed_dir):
        shutil.rmtree(processed_dir)
    
    # copy files
    shutil.copytree(unzipped_dir, processed_dir_legacy)
    # shutil.copytree(unzipped_dir, processed_dir_newer)

# convert efcproj to efkefc
def convert_to_efkefc(tool_dir, target_dir):
    if not os.path.exists(tool_dir + "Effekseer.exe"):
        print("error: Effekseer.exe is not found at {} .".format(tool_dir))
        return
    
    # get efkproj files
    files = glob.glob(target_dir + "**/*.efkproj", recursive=True)
    cnt = 1

    # process 
    for file in files:
        efkproj = file
        pth = pathlib.Path(efkproj)
        efkefc = str(pth.parent) + "/" + str(pth.stem) + ".efkefc"

        # relative path from effekseer tool
        rel_efkproj = os.path.relpath(efkproj, tool_dir)
        rel_efkefc = os.path.relpath(efkefc, tool_dir)

        # execute effekseer tool in CUI mode
        # print("\r Converting...({}/{}) : {:s}".format(cnt, len(files), efkproj), end="")
        print("Converting...({}/{}) : {:s}".format(cnt, len(files), efkproj))
        cmd = [tool_dir + "Effekseer.exe", "-cui", "-in", rel_efkproj ,"-o", rel_efkefc]
        subprocess.run(cmd, cwd=tool_dir)

        cnt += 1

    print("\n Conversion completed!")


if __name__ == "__main__": 
    print("Unzip and copy resource files.")
    prepare_process()
    print("Unzip and copy completed.")

    print("Convert  efkproj to efkefc in legacy version.")
    convert_to_efkefc(tool_dir_legacy, processed_dir_legacy)
    # print("Convert efkproj to efkefc in newer version.")
    # convert_to_efkefc(tool_dir_newer, processed_dir_newer)

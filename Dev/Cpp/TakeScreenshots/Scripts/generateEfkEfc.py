import unzipping
import os
import pathlib
import shutil
import glob
import subprocess
import urllib.request

test_resources_download_urls = [
    "https://effekseer.github.io/contributes/Effekseer01.zip",
    "https://effekseer.github.io/contributes/NextSoft01.zip",
    "https://effekseer.github.io/contributes/Pierre01_130.zip",
    "https://effekseer.github.io/contributes/Pierre02_130.zip",
    "https://effekseer.github.io/contributes/MAGICALxSPIRAL.zip",
    "https://effekseer.github.io/contributes/Suzuki01.zip",
    "https://effekseer.github.io/contributes/AndrewFM01.zip",
    "https://effekseer.github.io/contributes/tktk01.zip",
    "https://effekseer.github.io/contributes/tktk02.zip",
    "https://effekseer.github.io/contributes/ProjectDanmakuGirls.zip",
    "https://effekseer.github.io/contributes/NitoriBox.zip",
    "https://effekseer.github.io/contributes/TouhouStrategy.zip"]

tool_download_url = "https://github.com/effekseer/Effekseer/releases/download/170/Effekseer170Win.zip"

processed_dir = "./processed/"
processed_dir_legacy = "./processed/1.7/"
processed_dir_newer = "./processed/1.8/"

tool_dir = "./Tool/"
tool_dir_legacy = "./Tool/1.7/"
tool_dir_newer = "./Tool/1.8/"

zipped_dir = "./zipData/"
unzipped_dir = "./unzipped/"

def download_tool():
    if os.path.exists(tool_dir_legacy):
        shutil.rmtree(tool_dir_legacy)

    chache_dir = tool_dir + "cache/"
    if os.path.exists(chache_dir):
        shutil.rmtree(chache_dir)
    os.makedirs(chache_dir)

    filename = chache_dir + os.path.basename(tool_download_url)
    urllib.request.urlretrieve(tool_download_url, filename)

    # unzip
    shutil.unpack_archive(filename, chache_dir)
    # move files
    shutil.move(chache_dir + "Tool/", tool_dir_legacy)

    # delete chache    
    shutil.rmtree(chache_dir)
    

def download_files():
    if os.path.exists(zipped_dir):
        shutil.rmtree(zipped_dir)
    
    os.mkdir(zipped_dir)

    i = 1
    for url in test_resources_download_urls:
        filename = zipped_dir + os.path.basename(url)
        print("Downloading test resources...({}/{}) : {}".format(i, len(test_resources_download_urls), url))
        urllib.request.urlretrieve(url, filename)
        i += 1
    

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
    print("Download tool.")
    download_tool()
    print("Download tool completed.")

    print("Download test resources.")
    download_files()
    print("Download test resources completed.")
    
    print("Unzip and copy resource files.")
    prepare_process()
    print("Unzip and copy completed.")

    print("Convert  efkproj to efkefc in legacy version.")
    convert_to_efkefc(tool_dir_legacy, processed_dir_legacy)
    # print("Convert efkproj to efkefc in newer version.")
    # convert_to_efkefc(tool_dir_newer, processed_dir_newer)

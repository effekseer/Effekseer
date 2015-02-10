import sys
import os
import shutil
import subprocess
import os
import os.path
import zipfile
import platform
import os.path



def cd(path):
	os.chdir(path)

def cdToScript():
	cd(os.path.dirname(__file__))

def wget( address ):
	version = sys.version_info
	if version[0] == 2:
		import urllib
		urllib.urlretrieve(address, os.path.basename(address))
	elif version[0] == 3:
		import urllib.request
		urllib.request.urlretrieve(address, os.path.basename(address))

def unzip(zip_filename):
	zip_file = zipfile.ZipFile(zip_filename, "r")
	for f in zip_file.namelist():

		if f.endswith('/'):
		        os.makedirs(f)
		else:
			version = sys.version_info
			if version[0] == 2:
				unzip_file = file(f, "wb")
				unzip_file.write(zip_file.read(f))
				unzip_file.close()
			elif version[0] == 3:
				unzip_file = open(f, "wb")
				unzip_file.write(zip_file.read(f))
				unzip_file.close();
	zip_file.close()


def rm(path):
	if os.path.exists(path):
		os.remove(path)

def rmdir(path):
	if os.path.exists(path):
		print("rmdir : " + path)
		shutil.rmtree(path)
	else:
		print("rmdir : not found " + path)


def isWin():
	return platform.system() == 'Windows'

def isMac():
	return platform.system() == 'Darwin'

def call( cmd ):
	""" call command line.
	"""

	print( cmd )
	p = subprocess.Popen(cmd, shell=True)
	ret = p.wait()
	print('')

def mkdir(path):
	if not os.path.exists(path):
		os.mkdir(path)

def copy(src,dst):
	print("copying from {0} to {1}".format(src, dst))
	shutil.copy(src,dst)




cdToScript()
cd(r'../')
rm(r'master.zip')
rmdir(r'glfw-master')
wget(r'https://github.com/glfw/glfw/archive/master.zip')
unzip(r'master.zip')

if isWin():
	cd(r"glfw-master")
else:
	mkdir(r"glfw_bin")
	cd(r"glfw_bin")

if isWin():
	call(r'cmake -G "Visual Studio 12" -D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF -D USE_INTERNAL_LOADER:BOOL=OFF ../glfw-master/')
	call(r'"C:\Program Files (x86)\MSBuild\12.0\Bin\msbuild" ALL_BUILD.vcxproj /p:configuration=Debug')
	call(r'"C:\Program Files (x86)\MSBuild\12.0\Bin\msbuild" ALL_BUILD.vcxproj /p:configuration=Release')
else:
	call(r'cmake -G "Unix Makefiles" -D BUILD_SHARED_LIBS:BOOL=OFF -D CMAKE_INSTALL_PREFIX=../Dev/Cpp ../glfw-master/')
	call(r'make install')

cd(r"../")

mkdir(r"Dev/Cpp/include/")
mkdir(r"Dev/Cpp/include/GLFW/")

mkdir(r"Dev/Cpp/lib/")
mkdir(r"Dev/Cpp/lib/Debug/")
mkdir(r"Dev/Cpp/lib/Release/")

if isWin():
	copy(r'glfw-master/include/GLFW/glfw3.h', r'Dev/Cpp/include/GLFW/')
	copy(r'glfw-master/include/GLFW/glfw3native.h', r'Dev/Cpp/include/GLFW')

	copy(r'glfw-master/src/Debug/glfw3.lib', r'Dev/Cpp/lib/Debug/')
	copy(r'glfw-master/src/Release/glfw3.lib', r'Dev/Cpp/lib/Release/')

else:
	copy(r'glfw-master/include/GLFW/glfw3.h', r'Dev/Cpp/include/GLFW/')
	copy(r'glfw-master/include/GLFW/glfw3native.h', r'Dev/Cpp/include/GLFW')
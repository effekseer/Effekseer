#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
import shutil
import subprocess
import os
import os.path
import zipfile
import platform
import os.path

def get_files(path):
	""" get files.
	"""

	def getlistdir(path,l):
		dirs = os.listdir(path)
		for d in dirs:
			newpath = os.path.join( path, d )
			if os.path.isdir( newpath ):
				getlistdir( newpath, l )
			else:
				l.append( newpath )
	
	ret = []
	getlistdir( path, ret )
	return ret


def copytreeWithExt(src,dst,exts):
	files = get_files(src)
	for _from in files:

		root, ext = os.path.splitext(_from)
		if not ext in exts:
			continue
		_to = dst + _from[len(src):]
		print(_from + '\t:\t' + _to)

		if not os.path.exists(os.path.dirname(_to)):
			os.makedirs(os.path.dirname(_to))
		
		shutil.copy(_from,_to)


def isWin():
	return platform.system() == 'Windows'

def isMac():
	return platform.system() == 'Darwin'

def call( cmd , env=None):
	""" call command line.
	"""

	print( cmd )
	p = subprocess.Popen(cmd, shell=True, env=env)
	ret = p.wait()
	print('')


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

def cd(path):
	os.chdir(path)

def cdToScript():
	cd(os.path.dirname(os.path.abspath(__file__)))

def mkdir(path):
	if not os.path.exists(path):
		os.mkdir(path)

def copy(src,dst):
	print("copying from {0} to {1}".format(src, dst))
	shutil.copy(src,dst)

	
def copytree(src,dst,change=False,ignoreList = None):
	print("copying tree from {0} to {1}".format(src, dst))
	if change and os.path.exists(dst):
		rmdir(dst)

	if not os.path.exists(dst):
		shutil.copytree(src,dst,ignore=ignoreList)

def editCmakeForACE(path,enc='utf-8'):
	# This script edits a cmake file for ACE.
	# The script appends fPIC to compile on 64bit *nix OS.
	f = open(path, 'r', encoding=enc)
	lines = f.read()
	f.close()

	if '# ForACE' in lines:
		return

	# to glfw
	if 'GLFW' in lines:
		lines = lines.replace('-Wall', '-Wall -fPIC')

	# libGD
	if 'PROJECT(GD)' in lines:
		lines = lines.replace('FIND_PACKAGE(ZLIB)', '')
		lines = lines.replace('ENDIF(ZLIB_FOUND)', '')
		lines = lines.replace('IF(ZLIB_FOUND)', '')
		lines = lines.replace('INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})', '')
		lines = lines.replace('SET(HAVE_LIBZ 1)', '')

	lines = lines + "\n"
	lines = lines + "# ForACE\n"
	lines = lines + "if (MSVC)\n"
	lines = lines + "else()\n"
	lines = lines + "\tadd_definitions(-fPIC)\n"
	lines = lines + "\tset(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} -fPIC\")\n"
	lines = lines + "\tset(CMAKE_C_FLAGS_DEBUG \"${CMAKE_C_FLAGS_DEBUG} -fPIC\")\n"
	lines = lines + "\tset(CMAKE_C_FLAGS_RELEASE \"${CMAKE_C_FLAGS_RELEASE} -fPIC\")\n"
	lines = lines + "\tset(CMAKE_C_FLAGS_MINSIZEREL \"${CMAKE_C_FLAGS_MINSIZEREL} -fPIC\")\n"
	lines = lines + "\tset(CMAKE_C_FLAGS_RELWITHDEBINFO \"${CMAKE_C_FLAGS_RELWITHDEBINFO} -fPIC\")\n"
	lines = lines + "\tset(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -fPIC\")\n"
	lines = lines + "endif()\n"
	
	
	if 'project (glew)' in lines or 'Box2D' in lines or 'freetype' in lines or 'PROJECT(GD)' in lines or 'SET (LIBGD_SRC_FILES' in lines:
		lines = lines + "if (MSVC)\n"
		lines = lines + "\tforeach (flag CMAKE_C_FLAGS\n"
		lines = lines + "\t\tCMAKE_C_FLAGS_DEBUG\n"
		lines = lines + "\t\tCMAKE_C_FLAGS_RELEASE\n"
		lines = lines + "\t\tCMAKE_CXX_FLAGS\n"
		lines = lines + "\t\tCMAKE_CXX_FLAGS_DEBUG\n"
		lines = lines + "\t\tCMAKE_CXX_FLAGS_RELEASE)\n"
		lines = lines + "\t\tif (${flag} MATCHES \"/MD\")\n"
		lines = lines + "\t\t\tstring(REGEX REPLACE \"/MD\" \"/MT\" ${flag} \"${${flag}}\")\n"
		lines = lines + "\t\tendif()\n"
		lines = lines + "\t\tif (${flag} MATCHES \"/MDd\")\n"
		lines = lines + "\t\t\tstring(REGEX REPLACE \"/MDd\" \"/MTd\" ${flag} \"${${flag}}\")\n"
		lines = lines + "\t\tendif()\n"
		lines = lines + "\tendforeach()\n"
		lines = lines + "endif()\n"

	f = open(path, 'w')
	f.write(lines)
	f.close()

# strings
cmd_cmake = r'cmake -G "Visual Studio 14" '
cmd_cmake_x64 = r'cmake -G "Visual Studio 14 Win64" '
cmd_compile = r'"C:\Program Files (x86)\MSBuild\14.0\bin\msbuild" '
cmd_premake5 = os.path.dirname(os.path.abspath(__file__)) + r'/premake5/windows/premake5 vs2015 '

premake5_directory = 'vs2015'

def find_msbuild2017():
	if os.path.exists(r'C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\bin\MSBuild.exe'):
		return r'"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\bin\MSBuild.exe" '

	return r'"C:\Program Files (x86)\MSBuild\15.0\Bin\msbuild" '

if isWin():
	if(len(sys.argv) == 4):
		# すべて直接指定する
		cmd_cmake = r'cmake -G "' + sys.argv[1] + r'" ';
		cmd_cmake_x64 = r'cmake -G "' + sys.argv[2] + r'" ';
		cmd_compile = r'"' + sys.argv[3] + r'" ';
	elif(len(sys.argv) == 2 and sys.argv[1] == 'Visual Studio 15'):
		cmd_cmake = r'cmake -G "Visual Studio 15" '
		cmd_cmake_x64 = r'cmake -G "Visual Studio 15 Win64" '
		cmd_compile = find_msbuild2017()
		premake5_directory = 'vs2017'

	elif (len(sys.argv) == 1) or (len(sys.argv) == 2 and sys.argv[1] == 'Visual Studio 14'):
		# vs2015
		cmd_cmake = r'cmake -G "Visual Studio 14" '
		cmd_cmake_x64 = r'cmake -G "Visual Studio 14 Win64" '
		cmd_compile = r'"C:\Program Files (x86)\MSBuild\14.0\Bin\msbuild" '
	elif (len(sys.argv) == 2 and sys.argv[1] == 'Visual Studio 12'):
		# vs2013
		cmd_cmake = r'cmake -G "Visual Studio 12" '
		cmd_cmake_x64 = r'cmake -G "Visual Studio 12 Win64" '
		cmd_compile = r'"C:\Program Files (x86)\MSBuild\12.0\Bin\msbuild" '

	if 'Visual Studio 15' in sys.argv:
		cmd_premake5 = os.path.dirname(os.path.abspath(__file__)) + r'/premake5/windows/premake5 vs2017 '
		premake5_directory = 'vs2017'

elif isMac():
	cmd_premake5 = os.path.dirname(os.path.abspath(__file__)) + r'/premake5/macosx/premake5 '
	
class CurrentDir:
	def __init__(self, path):
		self.prev = os.getcwd()
		self.path = path
	def __enter__(self):
		cd(self.path)
		#print("cd: " + os.getcwd())
		return self
	def __exit__(self, type, value, traceback):
		cd(self.prev)
		#print("cd: " + os.getcwd())

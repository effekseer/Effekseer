
import aceutils
import itertools

aceutils.cdToScript()
aceutils.mkdir('../Downloads')

with aceutils.CurrentDir('../Downloads'):
	aceutils.rmdir(r"osm_bin")
	aceutils.rmdir(r"osm_bin_x64")

	aceutils.mkdir(r"osm_bin")
	aceutils.mkdir(r"osm_bin_x64")

	with aceutils.CurrentDir('osm_bin'):
		if aceutils.isWin():
			aceutils.call(aceutils.cmd_cmake+r'-D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF ../OpenSoundMixer/')
			aceutils.call(aceutils.cmd_compile + r'OpenSoundMixer.sln /p:configuration=Debug')
			aceutils.call(aceutils.cmd_compile + r'OpenSoundMixer.sln /p:configuration=Release')
		elif aceutils.isMac():
			aceutils.call(r'cmake -G "Unix Makefiles" "-DCMAKE_OSX_ARCHITECTURES=x86_64;i386" "-DCMAKE_OSX_DEPLOYMENT_TARGET=10.10" -D CMAKE_BUILD_TYPE=Release ../OpenSoundMixer/')
			aceutils.call(r'make')
		else:
			aceutils.call(r'cmake -G "Unix Makefiles" ../OpenSoundMixer/')
			aceutils.call(r'make')

	with aceutils.CurrentDir('osm_bin_x64'):
		if aceutils.isWin():
			aceutils.call(aceutils.cmd_cmake_x64+r'-D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF ../OpenSoundMixer/')
			aceutils.call(aceutils.cmd_compile + r'OpenSoundMixer.sln /p:configuration=Debug')
			aceutils.call(aceutils.cmd_compile + r'OpenSoundMixer.sln /p:configuration=Release')

	aceutils.mkdir(r"../Dev/Cpp/include/")
	aceutils.mkdir(r"../Dev/Cpp/lib/")

	if aceutils.isWin():
		aceutils.copy(r'OpenSoundMixer/src/OpenSoundMixer.h', r'../Dev/Cpp/include/')

		aceutils.copy(r'osm_bin/Debug/OpenSoundMixer.lib', r'../Dev/Cpp/lib/x86/Debug/')
		aceutils.copy(r'osm_bin/Release/OpenSoundMixer.lib', r'../Dev/Cpp/lib/x86/Release/')
		
		aceutils.copy(r'osm_bin_x64/Debug/OpenSoundMixer.lib', r'../Dev/Cpp/lib/x64/Debug/')
		aceutils.copy(r'osm_bin_x64/Release/OpenSoundMixer.lib', r'../Dev/Cpp/lib/x64/Release/')

	else:
		aceutils.copy(r'OpenSoundMixer/src/OpenSoundMixer.h', r'../Dev/Cpp/include/')
		aceutils.copy(r'osm_bin/lib/libOpenSoundMixer.a', r'../Dev/Cpp/lib/')

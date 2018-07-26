import aceutils

pathname = 'glfw'

aceutils.cdToScript()
aceutils.mkdir('../Downloads')

with aceutils.CurrentDir('../Downloads'):
	aceutils.rmdir(r"glfw_bin")
	aceutils.rmdir(r"glfw_bin_x64")

	aceutils.editCmakeForACE(pathname + r'/CMakeLists.txt')
	aceutils.mkdir(r"glfw_bin")
	aceutils.mkdir(r"glfw_bin_x64")
	aceutils.mkdir(r'glfw_bin/i386')

	with aceutils.CurrentDir('glfw_bin'):
		if aceutils.isWin():
			aceutils.call(aceutils.cmd_cmake+r'-D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF -D USE_INTERNAL_LOADER:BOOL=OFF ../' + pathname + '/')
			aceutils.call(aceutils.cmd_compile + r'ALL_BUILD.vcxproj /p:configuration=Debug')
			aceutils.call(aceutils.cmd_compile + r'ALL_BUILD.vcxproj /p:configuration=Release')
		elif aceutils.isMac():
			aceutils.call(r'cmake -G "Unix Makefiles" -D BUILD_SHARED_LIBS:BOOL=OFF -D CMAKE_INSTALL_PREFIX:PATH=../../Dev/Cpp "-DCMAKE_OSX_ARCHITECTURES=x86_64;i386" "-DCMAKE_OSX_DEPLOYMENT_TARGET=10.10" -D CMAKE_BUILD_TYPE=Release ../' + pathname + '/')
			aceutils.call(r'make install')
		else:
			aceutils.call(r'cmake -G "Unix Makefiles" -D BUILD_SHARED_LIBS:BOOL=OFF -D CMAKE_INSTALL_PREFIX=../../Dev/Cpp ../' + pathname + '/')
			aceutils.call(r'make install')

	with aceutils.CurrentDir('glfw_bin_x64'):
		if aceutils.isWin():
			aceutils.call(aceutils.cmd_cmake_x64+r'-D USE_MSVC_RUNTIME_LIBRARY_DLL:BOOL=OFF -D USE_INTERNAL_LOADER:BOOL=OFF ../' + pathname + '/')
			aceutils.call(aceutils.cmd_compile + r'ALL_BUILD.vcxproj /p:configuration=Debug')
			aceutils.call(aceutils.cmd_compile + r'ALL_BUILD.vcxproj /p:configuration=Release')
	
	aceutils.mkdir(r"../Dev/Cpp/include/")
	aceutils.mkdir(r"../Dev/Cpp/lib/")

	aceutils.mkdir(r"../Dev/Cpp/include/GLFW/")

	if aceutils.isWin():

		aceutils.mkdir(r'../Dev/Cpp/lib/x86/')
		aceutils.mkdir(r'../Dev/Cpp/lib/x86/Debug')
		aceutils.mkdir(r'../Dev/Cpp/lib/x86/Release')

		aceutils.mkdir(r'../Dev/Cpp/lib/x64/')
		aceutils.mkdir(r'../Dev/Cpp/lib/x64/Debug')
		aceutils.mkdir(r'../Dev/Cpp/lib/x64/Release')

		aceutils.copy(pathname + r'/include/GLFW/glfw3.h', r'../Dev/Cpp/include/GLFW/')
		aceutils.copy(pathname + r'/include/GLFW/glfw3native.h', r'../Dev/Cpp/include/GLFW')

		aceutils.copy(r'glfw_bin/src/Debug/glfw3.lib', r'../Dev/Cpp/lib/x86/Debug/')
		aceutils.copy(r'glfw_bin/src/Release/glfw3.lib', r'../Dev/Cpp/lib/x86/Release/')

		aceutils.copy(r'glfw_bin_x64/src/Debug/glfw3.lib', r'../Dev/Cpp/lib/x64/Debug/')
		aceutils.copy(r'glfw_bin_x64/src/Release/glfw3.lib', r'../Dev/Cpp/lib/x64/Release/')

	else:
		aceutils.copy(pathname + r'/include/GLFW/glfw3.h', r'../Dev/Cpp/include/GLFW/')
		aceutils.copy(pathname + r'/include/GLFW/glfw3native.h', r'../Dev/Cpp/include/GLFW')

		aceutils.copy(r'glfw_bin/src/libglfw3.a', r'../Dev/Cpp/lib')

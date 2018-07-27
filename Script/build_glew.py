import aceutils

pathname = 'glew-1.13.0'

aceutils.cdToScript()
aceutils.mkdir('../Downloads')

with aceutils.CurrentDir('../Downloads'):
	aceutils.rm(r'glew-1.13.0.zip')
	aceutils.rmdir(pathname)
	aceutils.rmdir(r"glew_bin")
	aceutils.rmdir(r"glew_bin_x64")

	aceutils.wget(r'https://sourceforge.net/projects/glew/files/glew/1.13.0/glew-1.13.0.zip')
	aceutils.unzip(r'glew-1.13.0.zip')
	aceutils.editCmakeForACE(pathname + r'/build/cmake/CMakeLists.txt')
	aceutils.mkdir(r"glew_bin")
	aceutils.mkdir(r"glew_bin_x64")

	with aceutils.CurrentDir('glew_bin'):
		if aceutils.isWin():
			aceutils.call(aceutils.cmd_cmake+r'-UGLEW_USE_STATIC_LIBS ../' + pathname + '/build/cmake')
			aceutils.call(aceutils.cmd_compile + r'ALL_BUILD.vcxproj /p:configuration=Debug')
			aceutils.call(aceutils.cmd_compile + r'ALL_BUILD.vcxproj /p:configuration=Release')
		elif aceutils.isMac():
			aceutils.call(r'cmake -G "Unix Makefiles" -UGLEW_USE_STATIC_LIBS -D CMAKE_INSTALL_PREFIX:PATH=../dev "-DCMAKE_OSX_ARCHITECTURES=x86_64;i386" "-DCMAKE_OSX_DEPLOYMENT_TARGET=10.10" -D CMAKE_BUILD_TYPE=Release ../' + pathname + '/build/cmake')
			aceutils.call(r'make install')
		else:
			aceutils.call(r'cmake -G "Unix Makefiles" -UGLEW_USE_STATIC_LIBS -D CMAKE_INSTALL_PREFIX=../dev ../' + pathname + '/build/cmake')
			aceutils.call(r'make install')

	with aceutils.CurrentDir('glew_bin_x64'):
		if aceutils.isWin():
			aceutils.call(aceutils.cmd_cmake_x64+r'-UGLEW_USE_STATIC_LIBS ../' + pathname + '/build/cmake')
			aceutils.call(aceutils.cmd_compile + r'ALL_BUILD.vcxproj /p:configuration=Debug')
			aceutils.call(aceutils.cmd_compile + r'ALL_BUILD.vcxproj /p:configuration=Release')

	aceutils.mkdir(r"../Dev/Cpp/include/")
	aceutils.mkdir(r"../Dev/Cpp/lib/")
	aceutils.mkdir(r'../Dev/Cpp/include/GL')

	if aceutils.isWin():
		aceutils.mkdir(r'../Dev/Cpp/lib/')
		aceutils.mkdir(r'../Dev/Cpp/lib/x86/')
		aceutils.mkdir(r'../Dev/Cpp/lib/x86/Debug')
		aceutils.mkdir(r'../Dev/Cpp/lib/x86/Release')

		aceutils.mkdir(r'../Dev/Cpp/lib/x64/')
		aceutils.mkdir(r'../Dev/Cpp/lib/x64/Debug')
		aceutils.mkdir(r'../Dev/Cpp/lib/x64/Release')

		aceutils.copy(pathname + r'/include/GL/glew.h', r'../Dev/Cpp/include/GL/')
		aceutils.copy(pathname + r'/include/GL/glxew.h', r'../Dev/Cpp/include/GL/')
		aceutils.copy(pathname + r'/include/GL/wglew.h', r'../Dev/Cpp/include/GL/')

		aceutils.copy(r'glew_bin/lib/Debug/libglew32d.lib', r'../Dev/Cpp/lib/x86/Debug/')
		aceutils.copy(r'glew_bin/lib/Release/libglew32.lib', r'../Dev/Cpp/lib/x86/Release/')

		aceutils.copy(r'glew_bin_x64/lib/Debug/libglew32d.lib', r'../Dev/Cpp/lib/x64/Debug/')
		aceutils.copy(r'glew_bin_x64/lib/Release/libglew32.lib', r'../Dev/Cpp/lib/x64/Release/')
	else:
		aceutils.copy(pathname + r'/include/GL/glew.h', r'../Dev/Cpp/include/GL/')
		aceutils.copy(pathname + r'/include/GL/glxew.h', r'../Dev/Cpp/include/GL/')
		aceutils.copy(pathname + r'/include/GL/wglew.h', r'../Dev/Cpp/include/GL/')
		aceutils.copy(r'glew_bin/lib/libGLEW.a', r'../Dev/Cpp/lib/')

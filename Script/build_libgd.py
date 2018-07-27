
import aceutils

originalpathname = r'libgd'
pathname = r'libgd_cp'

aceutils.cdToScript()

aceutils.rmdir(r"../Downloads/" + pathname)
aceutils.copytree(r"../Downloads/" + originalpathname, r"../Downloads/" + pathname)

with aceutils.CurrentDir('../Downloads'):
	aceutils.rmdir(r"gd_bin")
	aceutils.rmdir(r"gd_bin_x64")

	aceutils.editCmakeForACE(pathname + r'/CMakeLists.txt','cp932')
	aceutils.editCmakeForACE(pathname + r'/src/CMakeLists.txt','cp932')
	aceutils.mkdir(r"gd_bin")
	aceutils.mkdir(r"gd_bin_x64")

	with aceutils.CurrentDir('gd_bin'):
		if aceutils.isWin():
			aceutils.call(aceutils.cmd_cmake + r'-DBUILD_STATIC_LIBS=ON ../' + pathname + '/')
			aceutils.call(aceutils.cmd_compile + r'src/libgd_static.vcxproj /p:configuration=Debug')
			aceutils.call(aceutils.cmd_compile + r'src/libgd_static.vcxproj /p:configuration=Release')
		elif aceutils.isMac():
			aceutils.call(r'cmake -G "Unix Makefiles" -DBUILD_STATIC_LIBS=ON "-DCMAKE_OSX_ARCHITECTURES=x86_64;i386" "-DCMAKE_OSX_DEPLOYMENT_TARGET=10.10" -D CMAKE_BUILD_TYPE=Release ../' + pathname + '/')
			aceutils.call(r'make')
		else:
			aceutils.call(r'cmake -G "Unix Makefiles" -DBUILD_STATIC_LIBS=ON ../' + pathname + '/')
			aceutils.call(r'make')

	with aceutils.CurrentDir('gd_bin_x64'):
		if aceutils.isWin():
			aceutils.call(aceutils.cmd_cmake_x64 + r'-DBUILD_STATIC_LIBS=ON ../' + pathname + '/')
			aceutils.call(aceutils.cmd_compile + r'src/libgd_static.vcxproj /p:configuration=Debug')
			aceutils.call(aceutils.cmd_compile + r'src/libgd_static.vcxproj /p:configuration=Release')

	aceutils.mkdir(r"../Dev/Cpp/include/")
	aceutils.mkdir(r"../Dev/Cpp/lib/")

	if aceutils.isWin():
		aceutils.mkdir(r'../Dev/Cpp/lib/x86/')
		aceutils.mkdir(r'../Dev/Cpp/lib/x86/Debug')
		aceutils.mkdir(r'../Dev/Cpp/lib/x86/Release')

		aceutils.mkdir(r'../Dev/Cpp/lib/x64/')
		aceutils.mkdir(r'../Dev/Cpp/lib/x64/Debug')
		aceutils.mkdir(r'../Dev/Cpp/lib/x64/Release')

		aceutils.mkdir(r'../Dev/Cpp/include/gd')

		aceutils.copytreeWithExt(pathname + r'/src/', r'../Dev/Cpp/include/gd/',['.h'])

		aceutils.copy(r'gd_bin/Bin/Debug/libgd_static.lib', r'../Dev/Cpp/lib/x86/Debug/')
		aceutils.copy(r'gd_bin/Bin/Release/libgd_static.lib', r'../Dev/Cpp/lib/x86/Release/')
		aceutils.copy(r'gd_bin_x64/Bin/Debug/libgd_static.lib', r'../Dev/Cpp/lib/x64/Debug/')
		aceutils.copy(r'gd_bin_x64/Bin/Release/libgd_static.lib', r'../Dev/Cpp/lib/x64/Release/')

	else:
		aceutils.copytreeWithExt(pathname + r'/src/', r'../Dev/Cpp/include/gd/',['.h'])
		aceutils.copy(r'gd_bin/Bin/libgd.a', r'../Dev/Cpp/lib/')



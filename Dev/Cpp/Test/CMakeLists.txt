# Debug Releaseに限定
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE )

# プロジェクト名
project(Test)

# CMake バージョンチェック
cmake_minimum_required(VERSION 2.8)

# ソースコード設定
set(effekseer_src
	graphics_gl.cpp
	main.cpp)

# インクルードの設定
include_directories(
	../Effekseer/
	../EffekseerRendererGL/
	)

# コンパイル先を追加
add_executable( effekseer ${effekseer_src} )

# フラグ処理
add_definitions(-D__EFFEKSEER_RENDERER_GL_GLEW__)

# 出力先の設定
if (MSVC)
set_target_properties(effekseer PROPERTIES OUTPUT_NAME_DEBUG "Test" )
set_target_properties(effekseer PROPERTIES OUTPUT_NAME_RELEASE "Test" )
else()
set_target_properties(effekseer PROPERTIES OUTPUT_NAME "Test" )
endif()

install(
	TARGETS effekseer
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION bin
	LIBRARY DESTINATION bin)


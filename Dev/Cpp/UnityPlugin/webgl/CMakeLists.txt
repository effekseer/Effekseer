# =========================
# Effekseer UnityPlugin for WebGL
# =========================

# プロジェクト名
project(Effekseer)

# CMake バージョンチェック
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11" ${CMAKE_CXX_FLAGS})

# include 一覧を指定する
set(
	INCLUDE_DIR
	"${PROJECT_SOURCE_DIR}./../opengl"
	"${PROJECT_SOURCE_DIR}./../../Effekseer"
	"${PROJECT_SOURCE_DIR}./../../EffekseerRendererCommon"
	"${PROJECT_SOURCE_DIR}./../../EffekseerRendererGL")
	
include_directories (${INCLUDE_DIR}) 

# ソースファイル一覧を指定する
file(GLOB effekseer_src
	${PROJECT_SOURCE_DIR}./../../Effekseer/*.cpp
	${PROJECT_SOURCE_DIR}./../../Effekseer/Effekseer/*.cpp
	${PROJECT_SOURCE_DIR}./../../EffekseerRendererCommon/*.cpp
	${PROJECT_SOURCE_DIR}./../../EffekseerRendererGL/EffekseerRenderer/*.cpp
	${PROJECT_SOURCE_DIR}./../common/*.cpp
	${PROJECT_SOURCE_DIR}./../opengl/*.cpp)

# 使わないソースファイルを削除
# INTERNAL_LOADER
list(REMOVE_ITEM effekseer_src "${PROJECT_SOURCE_DIR}./../../EffekseerRendererCommon/EffekseerRenderer.DXTK.DDSTextureLoader.cpp")
list(REMOVE_ITEM effekseer_src "${PROJECT_SOURCE_DIR}./../../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.cpp")

# 拡張子を指定する
set(CMAKE_SHARED_LIBRARY_SUFFIX ".bc")

# コンパイル先を追加
add_library(libEffekseerUnity SHARED ${effekseer_src} )

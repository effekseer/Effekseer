project(fbxToEffekseerCurveConverter LANGUAGES C CXX)

add_executable(
  fbxToEffekseerCurveConverter
  source/FbxCurveConverter.cpp source/FbxCurveConverter.h source/FbxLoader.cpp
  source/FbxLoader.h source/Main.cpp)

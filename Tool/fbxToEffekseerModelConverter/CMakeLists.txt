project(fbxToEffekseerModelConverter LANGUAGES C CXX)

add_executable(
  fbxToEffekseerModelConverter fbxToEfkMdl.FBXConverter.cpp
                               fbxToMdl.VertexAnimation.cpp Utils.cpp main.cpp)

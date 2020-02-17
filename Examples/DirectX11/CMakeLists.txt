cmake_minimum_required (VERSION 3.0.0)

add_executable(Example_DirectX11 main.cpp)

add_definitions(-DEFK_EXAMPLE_ASSETS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../Resources/\")
add_definitions(-DEFK_EXAMPLE_ASSETS_DIR_U16=u\"${CMAKE_CURRENT_SOURCE_DIR}/../Resources/\")

target_include_directories(
    Example_DirectX11
    PRIVATE
	../../Dev/Cpp/Effekseer
	../../Dev/Cpp/EffekseerRendererDX11
	../../Dev/Cpp/EffekseerSoundXAudio2
	../Utils
)

target_link_libraries(
    Example_DirectX11
    PRIVATE
	Effekseer
    EffekseerRendererDX11
	EffekseerSoundXAudio2
	Example_Utils
)
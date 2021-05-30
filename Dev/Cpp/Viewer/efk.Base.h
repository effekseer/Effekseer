
#pragma once

#include <stdint.h>
#include <stdio.h>

namespace efk
{
enum class DeviceType
{
	DirectX11,
	OpenGL,
};
}

namespace Effekseer
{
namespace Tool
{

enum class DistortionType : int32_t
{
	Current,
	Effekseer120,
	Disabled,
};

enum class RenderingMethodType
{
	Normal,
	Wireframe,
	NormalWithWireframe,
	Overdraw,
};

} // namespace Tool
} // namespace Effekseer

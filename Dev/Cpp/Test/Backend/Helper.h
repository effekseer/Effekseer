#pragma once

#include <array>
#include <stdint.h>

struct SimpleVertex
{
	std::array<float, 3> Position;
	std::array<float, 2> UV;
	std::array<uint8_t, 4> Color;
};

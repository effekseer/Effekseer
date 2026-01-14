#pragma once

#include "EfkRes.Math.h"
#include <array>
#include <stdint.h>
#include <vector>

namespace efkres
{

struct Curve
{
	std::vector<Vec4> controllPoints;
	std::vector<double> knotValues;
	int order = 0;
	int step = 0;
	int type = 0;
	int dimension = 0;
};

} // namespace efkres

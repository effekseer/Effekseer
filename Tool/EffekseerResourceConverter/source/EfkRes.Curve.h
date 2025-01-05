#pragma once

#include <stdint.h>
#include <array>
#include <vector>
#include "EfkRes.Math.h"

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

}

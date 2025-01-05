#pragma once

#include <stdint.h>
#include <string_view>
#include <optional>
#include "EfkRes.Curve.h"

namespace efkres
{

class EfkCurveSaver
{
public:
	EfkCurveSaver() = default;
	~EfkCurveSaver() = default;

	bool Save(std::string_view filepath, const Curve& curve);

private:
	double m_scale = 1.0;

};

}

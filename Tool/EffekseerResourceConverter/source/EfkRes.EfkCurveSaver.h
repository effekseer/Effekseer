#pragma once

#include "EfkRes.Curve.h"
#include <optional>
#include <stdint.h>
#include <string_view>

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

} // namespace efkres

#pragma once

#include "EfkRes.Curve.h"
#include "EfkRes.Model.h"
#include <optional>
#include <stdint.h>
#include <string_view>

namespace efkres
{

class GEOLoader
{
public:
	GEOLoader() = default;
	~GEOLoader() = default;

	std::optional<Model> LoadModel(std::string_view filepath);
};

} // namespace efkres

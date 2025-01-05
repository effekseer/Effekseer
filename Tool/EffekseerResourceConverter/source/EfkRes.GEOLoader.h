#pragma once

#include <stdint.h>
#include <string_view>
#include <optional>
#include "EfkRes.Model.h"
#include "EfkRes.Curve.h"

namespace efkres
{

class GEOLoader
{
public:
	GEOLoader() = default;
	~GEOLoader() = default;

	std::optional<Model> LoadModel(std::string_view filepath);
};

}

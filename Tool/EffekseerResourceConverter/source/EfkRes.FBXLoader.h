#pragma once

#include <stdint.h>
#include <string_view>
#include <optional>
#include "EfkRes.Model.h"
#include "EfkRes.Curve.h"

namespace efkres
{

class FBXLoader
{
public:
	FBXLoader() = default;
	~FBXLoader() = default;

	std::optional<Model> LoadModel(std::string_view filepath);
	std::optional<Curve> LoadCurve(std::string_view filepath);
};

}

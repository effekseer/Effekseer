#pragma once

#include "EfkRes.Curve.h"
#include "EfkRes.Model.h"
#include <optional>
#include <stdint.h>
#include <string_view>
#include <vector>

namespace efkres
{

class FBXLoader
{
public:
	FBXLoader() = default;
	~FBXLoader() = default;

	std::optional<Model> LoadModel(std::string_view filepath);
	std::optional<std::vector<Model>> LoadModelSequence(std::string_view filepath);
	std::optional<Curve> LoadCurve(std::string_view filepath);
};

} // namespace efkres

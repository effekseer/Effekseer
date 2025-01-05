#pragma once

#include <stdint.h>
#include <string_view>
#include <optional>
#include "EfkRes.Model.h"

namespace efkres
{

class FBXLoader
{
public:
	FBXLoader() = default;
	~FBXLoader() = default;

	std::optional<Model> Load(std::string_view filepath);
};

}

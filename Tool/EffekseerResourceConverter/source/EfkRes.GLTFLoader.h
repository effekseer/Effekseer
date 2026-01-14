#pragma once

#include "EfkRes.Model.h"
#include <optional>
#include <stdint.h>
#include <string_view>

namespace efkres
{

class GLTFLoader
{
public:
	GLTFLoader() = default;
	~GLTFLoader() = default;

	std::optional<Model> LoadModel(std::string_view filepath);
};

} // namespace efkres

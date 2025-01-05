#pragma once

#include <stdint.h>
#include <string_view>
#include <optional>
#include "EfkRes.Model.h"

namespace efkres
{

class GLTFLoader
{
public:
	GLTFLoader() = default;
	~GLTFLoader() = default;

	std::optional<Model> LoadModel(std::string_view filepath);
};

}

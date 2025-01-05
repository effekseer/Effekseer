#pragma once

#include <stdint.h>
#include <string_view>
#include <optional>
#include "EfkRes.Model.h"

namespace efkres
{

class MQOLoader
{
public:
	MQOLoader() = default;
	~MQOLoader() = default;

	std::optional<Model> LoadModel(std::string_view filepath);
};

}

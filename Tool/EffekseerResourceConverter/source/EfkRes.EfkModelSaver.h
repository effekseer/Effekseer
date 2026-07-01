#pragma once

#include "EfkRes.Model.h"
#include <optional>
#include <stdint.h>
#include <string_view>
#include <vector>

namespace efkres
{

class EfkModelSaver
{
public:
	EfkModelSaver() = default;
	~EfkModelSaver() = default;

	bool Save(std::string_view filepath, const Model& model);
	bool Save(std::string_view filepath, const std::vector<Model>& frames);

	void SetModelScale(double scale) { m_scale = scale; }

private:
	double m_scale = 1.0;
};

} // namespace efkres

#pragma once

#include <stdint.h>
#include <string_view>
#include <optional>
#include "EfkRes.Model.h"

namespace efkres
{

class EfkModelSaver
{
public:
	EfkModelSaver() = default;
	~EfkModelSaver() = default;

	bool Save(std::string_view filepath, const Model& model);

	void SetModelScale(double scale) { m_scale = scale; }

private:
	bool SaveAsVersion5(const char* filepath, const Model& model);

private:
	double m_scale = 1.0;

};

}

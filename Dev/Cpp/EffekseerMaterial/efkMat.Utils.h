
#pragma once

#include "efkMat.Base.h"

namespace EffekseerMaterial
{

std::string Replace(std::string v, std::string pre, std::string past);

struct Vector2DF
{
	float X;
	float Y;

	Vector2DF() : X(0), Y(0) {}

	Vector2DF(float x, float y) : X(x), Y(y) {}
};

std::string EspcapeUserParamName(const char* name);

std::string GetConstantTextureName(int64_t guid);

} // namespace EffekseerMaterial
#pragma once

#include <Effekseer.h>

namespace Effekseer
{
namespace Shader
{

std::string GetFixedGradient(const char* name, const Gradient& gradient);

const char* GetGradientFunctions();

const char* GetNoiseFunctions();

// TODO : support metal
const char* GetLinearGammaFunctions();

} // namespace Shader
} // namespace Effekseer
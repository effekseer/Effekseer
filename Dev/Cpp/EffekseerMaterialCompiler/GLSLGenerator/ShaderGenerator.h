
#pragma once

#include "../../Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.h"
#include <iostream>

namespace Effekseer
{
namespace GLSL
{

struct ShaderData
{
	std::string CodeVS;
	std::string CodePS;
};

class ShaderGenerator
{
	bool useUniformBlock_ = false;
	bool useSet_ = false;
	int32_t textureBindingOffset_ = 0;

	std::string Replace(std::string target, std::string from_, std::string to_);

	std::string GetType(int32_t i);

	std::string GetElement(int32_t i);

	void ExportUniform(std::ostringstream& maincode, int32_t type, const char* name);

	void ExportTexture(std::ostringstream& maincode, const char* name, int bind, int stage);

	void ExportHeader(std::ostringstream& maincode, MaterialFile* materialFile, int stage, bool isSprite, bool isOutputDefined, bool is450);

	void ExportDefaultUniform(std::ostringstream& maincode, MaterialFile* materialFile, int stage, bool isSprite);

	void ExportMain(std::ostringstream& maincode,
					MaterialFile* materialFile,
					int stage,
					bool isSprite,
					MaterialShaderType shaderType,
					const std::string& baseCode,
					bool useUniformBlock);

public:
	ShaderData GenerateShader(MaterialFile* materialFile,
							  MaterialShaderType shaderType,
							  int32_t maximumUniformCount,
							  int32_t maximumTextureCount,
							  bool useUniformBlock,
							  bool isOutputDefined,
							  bool is450,
							  bool useSet,
							  int textureBindingOffset,
							  bool isYInverted,
							  bool isScreenFlipped,
							  int instanceCount);
};

} // namespace GLSL

} // namespace Effekseer

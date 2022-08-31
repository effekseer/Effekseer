#pragma once

#include "../../Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.h"
#include <string>
#include <vector>

namespace Effekseer
{
namespace DirectX
{

struct ShaderData
{
	std::string CodeVS;
	std::string CodePS;
};

enum class ShaderGeneratorTarget
{
	DirectX9,
	DirectX11,
	DirectX12,
	PSSL,
	XBOXONE,
};

class ShaderGenerator
{
protected:
	std::string common_define_;
	std::string common_vs_define_;
	std::string sprite_vs_pre_;
	std::string sprite_vs_pre_simple_;
	std::string model_vs_pre_;
	std::string sprite_vs_suf1_;
	std::string sprite_vs_suf1_simple_;
	std::string model_vs_suf1_;
	std::string sprite_vs_suf2_;
	std::string model_vs_suf2_;
	std::string ps_pre_;
	std::string ps_suf1_;
	std::string ps_suf2_lit_;
	std::string ps_suf2_unlit_;
	std::string ps_suf2_refraction_;
	ShaderGeneratorTarget target_;

	std::string Replace(std::string target, std::string from_, std::string to_);

	std::string GetType(int32_t i);

	std::string GetElement(int32_t i);

	void ExportUniform(std::ostringstream& maincode, int32_t type, const char* name, int32_t registerId);

	void ExportTexture(std::ostringstream& maincode, const char* name, int32_t registerId);

	int32_t ExportHeader(std::ostringstream& maincode, MaterialFile* materialFile, int stage, bool isSprite, int instanceCount);

	void ExportMain(std::ostringstream& maincode,
					MaterialFile* materialFile,
					int stage,
					bool isSprite,
					MaterialShaderType shaderType,
					const std::string& baseCode);

public:
	ShaderGenerator(ShaderGeneratorTarget target);

	ShaderData GenerateShader(MaterialFile* materialFile,
							  MaterialShaderType shaderType,
							  int32_t maximumUniformCount,
							  int32_t maximumTextureCount,
							  int32_t pixelShaderTextureSlotOffset,
							  int32_t instanceCount);
};

} // namespace DirectX

} // namespace Effekseer
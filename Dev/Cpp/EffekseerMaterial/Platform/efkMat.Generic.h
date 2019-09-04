
#pragma once

#include "../efkMat.TextExporter.h"
#include "../efkMat.Utils.h"

namespace EffekseerMaterial
{

/**
	@brief	text for generic platforms
	@note
	$F1$ float1
	$F2$ float2
	$F3$ float3
	$F4$ float4

	$TEX_P{index}$ uv $TEX_S${index} sample texture

	$UV$ uv
	$TIME$ time
*/
class TextExporterGeneric : public TextExporter
{
private:
protected:
	std::string ExportNode(std::shared_ptr<TextExporterNode> node) override;

public:
	TextExporterGeneric() = default;
	virtual ~TextExporterGeneric() = default;

	std::string MergeTemplate(std::string code, std::string uniform_texture) override;
	std::string GetTypeName(ValueType type) const override;
	std::string GetUVName(int32_t ind) const override;
	std::string GetTimeName() const override;
	std::string ExportUniformAndTextures(const std::vector<std::shared_ptr<TextExporterUniform>>& uniformNodes,
										 const std::vector<std::shared_ptr<TextExporterTexture>>& textureNodes) override;
};

} // namespace EffekseerMaterial
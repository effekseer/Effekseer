
#pragma once

#include "../efkMat.TextExporter.h"
#include "../efkMat.Utils.h"

namespace EffekseerMaterial
{

class TextExporterGLSL : public TextExporter
{
private:
public:
	TextExporterGLSL() = default;
	virtual ~TextExporterGLSL() = default;

	std::string MergeTemplate(std::string code, std::string uniform_texture) override;

	std::string ExportNode(std::shared_ptr<TextExporterNode> node) override;

	std::string GetTypeName(ValueType type) const override;

	std::string GetUVName(int32_t ind) const override;

	std::string GetTimeName() const override;

	static std::string GetVertexShaderCode();
};

} // namespace EffekseerMaterial
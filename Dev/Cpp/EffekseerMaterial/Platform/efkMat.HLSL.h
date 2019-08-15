
#pragma once

#include "../efkMat.TextExporter.h"
#include "../efkMat.Utils.h"

namespace EffekseerMaterial
{

class TextExporterHLSL : public TextExporter
{
private:
public:
	TextExporterHLSL() = default;
	virtual ~TextExporterHLSL() = default;

	std::string MergeTemplate(std::string code, std::string uniform_texture) override;
	std::string GetTypeName(ValueType type) const override;
	std::string GetUVName() const override;
	std::string GetTimeName() const override;
};

} // namespace EffekseerMaterial
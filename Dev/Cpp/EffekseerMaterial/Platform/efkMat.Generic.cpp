#include "efkMat.Generic.h"
#include "../efkMat.Models.h"
#include "../efkMat.Parameters.h"

namespace EffekseerMaterial
{

std::string TextExporterGeneric::MergeTemplate(std::string code, std::string uniform_texture)
{
	const char template_[] = R"(

$F4$ Calculate($INPUT$)
{

RETURN

}

)";

	auto ret = Replace(template_, "RETURN", code);
	ret = Replace(ret.c_str(), "UNIFORM_TEXTURE", uniform_texture);

	return ret;
}

std::string TextExporterGeneric::GetTypeName(ValueType type) const
{
	if (type == ValueType::Float1)
		return "$F1$";
	if (type == ValueType::Float2)
		return "$F2$";
	if (type == ValueType::Float3)
		return "$F3$";
	if (type == ValueType::Float4)
		return "$F4$";
	return "";
}

std::string TextExporterGeneric::GetUVName() const { return "$UV$"; }

std::string TextExporterGeneric::GetTimeName() const { return "$TIME$"; }

std::string TextExporterGeneric::ExportUniformAndTextures(const std::vector<std::shared_ptr<TextExporterUniform>>& uniformNodes,
														  const std::vector<std::shared_ptr<TextExporterTexture>>& textureNodes)
{
	std::ostringstream ret;

	return ret.str();
}

std::string TextExporterGeneric::Export(std::shared_ptr<TextExporterNode> node)
{
	std::ostringstream ret;

	if (node->Target->Parameter->Type == NodeType::SampleTexture)
	{
		assert(node->Inputs[0].TextureValue != nullptr);
		if (0 <= node->Inputs[0].TextureValue->Index)
		{
			ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << " = $TEX_P" << node->Inputs[0].TextureValue->Index
				<< "$" << GetInputArg(ValueType::Float2, node->Inputs[1]) << "$TEX_S" << node->Inputs[0].TextureValue->Index  << "$;"
				<< std::endl;
		}
		else
		{
			ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "=" << GetTypeName(ValueType::Float4)
				<< "(1.0,1.0,1.0,1.0);" << std::endl;
		}

		return ret.str();
	}
	else
	{
		return TextExporter::Export(node);
	}
}

} // namespace EffekseerMaterial
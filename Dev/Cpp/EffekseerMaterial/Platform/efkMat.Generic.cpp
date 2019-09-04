#include "efkMat.Generic.h"
#include "../efkMat.Models.h"
#include "../efkMat.Parameters.h"

namespace EffekseerMaterial
{

std::string TextExporterGeneric::MergeTemplate(std::string code, std::string uniform_texture)
{
	const char template_[] = R"(

RETURN

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

std::string TextExporterGeneric::GetUVName(int32_t ind) const { 
	if (ind == 0)
	{
		return "$UV$1";
	}
	return "$UV$2";
}

std::string TextExporterGeneric::GetTimeName() const { return "$TIME$"; }

std::string TextExporterGeneric::ExportUniformAndTextures(const std::vector<std::shared_ptr<TextExporterUniform>>& uniformNodes,
														  const std::vector<std::shared_ptr<TextExporterTexture>>& textureNodes)
{
	std::ostringstream ret;

	return ret.str();
}

std::string TextExporterGeneric::ExportNode(std::shared_ptr<TextExporterNode> node)
{
	std::ostringstream ret;

	auto exportInputOrProp = [this](ValueType type_, TextExporterPin& pin_, std::shared_ptr<NodeProperty>& prop_) -> std::string {
		if (pin_.IsConnected)
		{
			return GetInputArg(type_, pin_);
		}
		return GetInputArg(pin_.Type, prop_->Floats[0]);
	};

	auto exportIn2Out2Param2 = [&, this](const char* func, const char* op) -> void {
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "=" << func << "("
			<< exportInputOrProp(node->Outputs[0].Type, node->Inputs[0], node->Target->Properties[0]) << op
			<< exportInputOrProp(node->Outputs[0].Type, node->Inputs[1], node->Target->Properties[1]) << ");" << std::endl;
	};

	if (node->Target->Parameter->Type == NodeType::FMod)
	{
		exportIn2Out2Param2("$MOD", ",");

		return ret.str();
	}
	else if (node->Target->Parameter->Type == NodeType::SampleTexture)
	{
		assert(node->Inputs[0].TextureValue != nullptr);
		if (0 <= node->Inputs[0].TextureValue->Index)
		{
			ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << " = $TEX_P" << node->Inputs[0].TextureValue->Index
				<< "$" << GetInputArg(ValueType::Float2, node->Inputs[1]) << "$TEX_S" << node->Inputs[0].TextureValue->Index << "$;"
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
		return TextExporter::ExportNode(node);
	}
}

} // namespace EffekseerMaterial
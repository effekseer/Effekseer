#include "efkMat.HLSL.h"

namespace EffekseerMaterial
{

std::string TextExporterHLSL::MergeTemplate(std::string code, std::string uniform_texture)
{
	const char template_[] = R"(

struct PS_Input
{
	float4 Position		: SV_POSITION;
	float4 Color		: COLOR;
	float2 UV			: TEXCOORD0;
};

UNIFORM

TEXTURE

float4 Calculate(const PS_Input input)
{

RETURN

}

float4 PS( const PS_Input input ) : SV_Target
{
	return Calculate(input);
}

)";

	auto ret = Replace(template_, "RETURN", code);
	ret = Replace(ret.c_str(), "UNIFORM_TEXTURE", uniform_texture);

	return ret;
}

std::string TextExporterHLSL::GetTypeName(ValueType type) const
{
	if (type == ValueType::Float1)
		return "float";
	if (type == ValueType::Float2)
		return "float2";
	if (type == ValueType::Float3)
		return "float3";
	if (type == ValueType::Float4)
		return "float4";
	return "";
}

std::string TextExporterHLSL::GetUVName() const { return "input.UV"; }

std::string TextExporterHLSL::GetTimeName() const { return "ps_time"; }


} // namespace EffekseerMaterial
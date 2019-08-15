#include "efkMat.GLSL.h"
#include "../efkMat.Models.h"
#include "../efkMat.Parameters.h"

namespace EffekseerMaterial
{

std::string TextExporterGLSL::MergeTemplate(std::string code, std::string uniform_texture)
{
	const char template_[] = R"(

in vec2 ps_UV;
in vec4 ps_VertexColor;

out vec4 out_Color;

UNIFORM_TEXTURE

uniform float ps_time;

vec4 Calculate()
{

RETURN

}

void main() 
{
	out_Color = Calculate();
}

)";

	auto ret = Replace(template_, "RETURN", code);
	ret = Replace(ret.c_str(), "UNIFORM_TEXTURE", uniform_texture);

	return ret;
}

std::string TextExporterGLSL::Export(std::shared_ptr<TextExporterNode> node) { return TextExporter::Export(node); }

std::string TextExporterGLSL::GetTypeName(ValueType type) const
{
	if (type == ValueType::Float1)
		return "float";
	if (type == ValueType::Float2)
		return "vec2";
	if (type == ValueType::Float3)
		return "vec3";
	if (type == ValueType::Float4)
		return "vec4";
	return "";
}

std::string TextExporterGLSL::GetUVName() const { return "ps_UV"; }

std::string TextExporterGLSL::GetTimeName() const { return "ps_time"; }

std::string TextExporterGLSL::GetVertexShaderCode()
{
	static auto code = R"(

in vec3 Pos;
in vec2 UV;
in vec4 Color;

out vec2 ps_UV;
out vec4 ps_VertexColor;

void main()
{
	vec4 pos = vec4(Pos.x,Pos.y,Pos.x,1.0);
	gl_Position = pos;

	ps_UV = vec2(UV.x,UV.y);
	ps_VertexColor = Color;
}

)";
	return code;
}

} // namespace EffekseerMaterial
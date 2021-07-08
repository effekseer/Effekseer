
#include "efk.LineRendererGL.h"

#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.GLExtension.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>

namespace efk
{

#include "../../../Shaders/GLSL_GL_Header/line_ps.h"
#include "../../../Shaders/GLSL_GL_Header/line_vs.h"

/*
static const char g_sprite_vs_src[] =
	R"(
IN vec4 atPosition;
IN vec4 atColor;
IN vec4 atTexCoord;
)"

	R"(
OUT vec4 vaColor;
OUT vec4 vaTexCoord;
OUT vec4 vaPos;
OUT vec4 vaPosR;
OUT vec4 vaPosU;
)"

	R"(
uniform mat4 uMatCamera;
uniform mat4 uMatProjection;

void main() {
	vec4 cameraPos = uMatCamera * atPosition;
	cameraPos = cameraPos / cameraPos.w;

	gl_Position = uMatProjection * cameraPos;

	vaPos = gl_Position;

	vec4 cameraPosU = cameraPos + vec4(0.0, 1.0, 0.0, 0.0);
	vec4 cameraPosR = cameraPos + vec4(1.0, 0.0, 0.0, 0.0);

	vaPosR = uMatProjection * cameraPosR;
	vaPosU = uMatProjection * cameraPosU;
	
	vaPos = vaPos / vaPos.w;
	vaPosR = vaPosR / vaPosR.w;
	vaPosU = vaPosU / vaPosU.w;

	vaColor = atColor;
	vaTexCoord = atTexCoord;
}

)";

static const char g_sprite_fs_texture_src[] = "IN lowp vec4 vaColor;\n"
											  "IN mediump vec4 vaTexCoord;\n"

											  "uniform sampler2D uTexture0;\n"

											  "void main() {\n"
											  "FRAGCOLOR = vaColor * TEX2D(uTexture0, vaTexCoord.xy);\n"
											  "}\n";

static const char g_sprite_fs_no_texture_src[] = "IN lowp vec4 vaColor;\n"
												 "IN mediump vec4 vaTexCoord;\n"

												 "void main() {\n"
												 "FRAGCOLOR = vaColor;\n"
												 "}\n";
*/

LineRendererGL::LineRendererGL(const EffekseerRenderer::RendererRef& renderer)
	: LineRenderer(renderer)
	, renderer(EffekseerRendererGL::RendererImplementedRef::FromPinned(renderer.Get()))
{
	EffekseerRendererGL::ShaderCodeView lineCodeDataVS(gl_line_vs);
	EffekseerRendererGL::ShaderCodeView lineCodeDataNPS(gl_line_ps);

	auto shader_no_texture_ =
		EffekseerRendererGL::Shader::Create(this->renderer->GetInternalGraphicsDevice(), &lineCodeDataVS, 1, &lineCodeDataNPS, 1, "Standard NoTex", true, false);

	EffekseerRendererGL::ShaderAttribInfo sprite_attribs[3] = {
		{"Input_Pos", GL_FLOAT, 3, 0, false}, {"Input_Color", GL_UNSIGNED_BYTE, 4, 12, true}, {"Input_UV", GL_FLOAT, 2, 16, false}};

	shader_no_texture_->GetAttribIdList(3, sprite_attribs);
	shader_no_texture_->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);

	shader_no_texture_->AddVertexConstantLayout(
		EffekseerRendererGL::CONSTANT_TYPE_MATRIX44, shader_no_texture_->GetUniformId("CBVS0.mCamera"), 0);

	shader_no_texture_->AddVertexConstantLayout(
		EffekseerRendererGL::CONSTANT_TYPE_MATRIX44, shader_no_texture_->GetUniformId("CBVS0.mProj"), sizeof(Effekseer::Matrix44));

	this->shader = shader_no_texture_;

	auto gd = this->renderer->GetGraphicsDevice().DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();

	vertexBuffer = EffekseerRendererGL::VertexBuffer::Create(gd, sizeof(EffekseerRendererGL::Vertex) * 1024, sizeof(EffekseerRendererGL::Vertex) * 1024, true);

	vao = EffekseerRendererGL::VertexArray::Create(
		gd, shader_no_texture_, (EffekseerRendererGL::VertexBuffer*)vertexBuffer, nullptr);
}

LineRendererGL::~LineRendererGL()
{
	auto shader_ = (EffekseerRendererGL::Shader*)shader;
	ES_SAFE_DELETE(shader_);
	ES_SAFE_DELETE(vao);
	ES_SAFE_DELETE(vertexBuffer);
	shader = nullptr;
}

void LineRendererGL::DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c)
{
	EffekseerRendererGL::Vertex v0;
	v0.Pos = p1;
	v0.SetColor(c, false);

	EffekseerRendererGL::Vertex v1;
	v1.Pos = p2;
	v1.SetColor(c, false);

	vertexies.push_back(v0);
	vertexies.push_back(v1);
}

void LineRendererGL::Render()
{
	GLCheckError();

	if (vertexies.size() == 0)
		return;

	vertexBuffer->Lock();

	auto vs = (EffekseerRendererGL::Vertex*)vertexBuffer->GetBufferDirect(sizeof(EffekseerRendererGL::Vertex) * vertexies.size());
	if (vs == nullptr)
		return;

	memcpy(vs, vertexies.data(), sizeof(EffekseerRendererGL::Vertex) * vertexies.size());

	vertexBuffer->Unlock();

	auto& state = renderer->GetRenderState()->Push();
	state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
	state.DepthWrite = true;
	state.DepthTest = true;
	state.CullingType = Effekseer::CullingType::Double;

	renderer->SetRenderMode(Effekseer::RenderMode::Normal);
	renderer->SetVertexArray(vao);
	renderer->BeginShader((EffekseerRendererGL::Shader*)shader);

	Effekseer::Backend::TextureRef textures[2];
	textures[0] = nullptr;
	textures[1] = nullptr;

	renderer->SetTextures((EffekseerRendererGL::Shader*)shader, textures, 1);

	Effekseer::Matrix44 constantVSBuffer[2];
	constantVSBuffer[0] = renderer->GetCameraMatrix();
	constantVSBuffer[1] = renderer->GetProjectionMatrix();

	constantVSBuffer[0].Transpose();
	constantVSBuffer[1].Transpose();

	renderer->SetVertexBufferToShader(constantVSBuffer, sizeof(Effekseer::Matrix44) * 2, 0);

	shader->SetConstantBuffer();

	renderer->GetRenderState()->Update(false);

	renderer->SetVertexBuffer((EffekseerRendererGL::VertexBuffer*)vertexBuffer, sizeof(EffekseerRendererGL::Vertex));
	// EffekseerRendererGL::GLExt::glBindBuffer(GL_ARRAY_BUFFER, renderer->GetVertexBuffer()->GetInterface());
	// EffekseerRendererGL::GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	renderer->SetLayout((EffekseerRendererGL::Shader*)shader);

	glDrawArrays(GL_LINES, 0, vertexies.size());

	renderer->EndShader((EffekseerRendererGL::Shader*)shader);
	renderer->SetVertexArray(nullptr);

	renderer->GetRenderState()->Pop();

	GLCheckError();
}

void LineRendererGL::ClearCache()
{
	vertexies.clear();
}

void LineRendererGL::OnLostDevice()
{
}

void LineRendererGL::OnResetDevice()
{
}
} // namespace efk


#include "efk.LineRendererGL.h"

#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.GLExtension.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>

namespace efk
{

#include "../../../Shaders/GLSL_GL_Header/line_ps.h"
#include "../../../Shaders/GLSL_GL_Header/line_vs.h"

LineRendererGL::LineRendererGL(const EffekseerRenderer::RendererRef& renderer)
	: LineRenderer(renderer)
	, renderer(EffekseerRendererGL::RendererImplementedRef::FromPinned(renderer.Get()))
{
	EffekseerRendererGL::ShaderCodeView lineCodeDataVS(gl_line_vs);
	EffekseerRendererGL::ShaderCodeView lineCodeDataNPS(gl_line_ps);

	using namespace Effekseer::Backend;
	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElm;
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, "CBVS0.mCamera", UniformBufferLayoutElementType::Matrix44, 1, 0});
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, "CBVS0.mProj", UniformBufferLayoutElementType::Matrix44, 1, sizeof(Effekseer::Matrix44)});
	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{}, uniformLayoutElm);

	auto shader_no_texture_ =
		EffekseerRendererGL::Shader::Create(this->renderer->GetInternalGraphicsDevice(), {lineCodeDataVS}, {lineCodeDataNPS}, uniformLayout, "Standard NoTex");

	const Effekseer::Backend::VertexLayoutElement vlElem[3] = {
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Pos", "POSITION", 0},
		{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "Input_Color", "NORMAL", 0},
		{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_UV", "TEXCOORD", 0},
	};

	auto vl = renderer->GetGraphicsDevice()->CreateVertexLayout(vlElem, 3).DownCast<EffekseerRendererGL::Backend::VertexLayout>();
	shader_no_texture_->SetVertexLayout(vl);

	shader_no_texture_->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);

	this->shader = shader_no_texture_;

	auto gd = this->renderer->GetGraphicsDevice().DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();

	vertexBuffer = EffekseerRendererGL::VertexBuffer::Create(gd, true, sizeof(EffekseerRendererGL::Vertex) * 1024, true);

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

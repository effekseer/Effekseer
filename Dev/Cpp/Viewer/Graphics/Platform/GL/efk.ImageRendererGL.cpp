
#include "efk.ImageRendererGL.h"

#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.GLExtension.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>

namespace efk
{

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

ImageRendererGL::ImageRendererGL(const EffekseerRenderer::RendererRef& renderer)
	: ImageRenderer(renderer)
	, renderer(EffekseerRendererGL::RendererImplementedRef::FromPinned(renderer.Get()))
{
	using namespace Effekseer::Backend;
	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElm;
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, "uMatCamera", UniformBufferLayoutElementType::Matrix44, 1, 0});
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Vertex, "uMatProjection", UniformBufferLayoutElementType::Matrix44, 1, sizeof(Effekseer::Matrix44)});
	auto uniformLayoutTex = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"uTexture0"}, uniformLayoutElm);
	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{}, uniformLayoutElm);

	EffekseerRendererGL::ShaderCodeView lineCodeDataVS(g_sprite_vs_src);
	EffekseerRendererGL::ShaderCodeView lineCodeDataPS(g_sprite_fs_texture_src);
	EffekseerRendererGL::ShaderCodeView lineCodeDataNPS(g_sprite_fs_no_texture_src);

	auto shader_ =
		EffekseerRendererGL::Shader::CreateWithHeader(this->renderer->GetInternalGraphicsDevice(), lineCodeDataVS, lineCodeDataPS, uniformLayoutTex, "Standard Tex");

	auto shader_no_texture_ =
		EffekseerRendererGL::Shader::CreateWithHeader(this->renderer->GetInternalGraphicsDevice(), lineCodeDataVS, lineCodeDataNPS, uniformLayout, "Standard NoTex");

	const Effekseer::Backend::VertexLayoutElement vlElem[3] = {
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "atPosition", "POSITION", 0},
		{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atColor", "NORMAL", 0},
		{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord", "TEXCOORD", 0},
	};

	auto vl = renderer->GetGraphicsDevice()->CreateVertexLayout(vlElem, 3).DownCast<EffekseerRendererGL::Backend::VertexLayout>();
	shader_->SetVertexLayout(vl);

	shader_->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);

	shader_no_texture_->SetVertexLayout(vl);
	shader_no_texture_->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);

	this->shader = shader_;
	this->shader_no_texture = shader_no_texture_;

	auto gd = this->renderer->GetGraphicsDevice().DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();

	vertexBuffer = EffekseerRendererGL::VertexBuffer::Create(gd, true, sizeof(EffekseerRendererGL::Vertex) * 12, false);

	vao = EffekseerRendererGL::VertexArray::Create(
		gd, shader_, (EffekseerRendererGL::VertexBuffer*)vertexBuffer, this->renderer->GetIndexBuffer());
	vao_nt = EffekseerRendererGL::VertexArray::Create(
		gd, shader_no_texture_, (EffekseerRendererGL::VertexBuffer*)vertexBuffer, this->renderer->GetIndexBuffer());
}

ImageRendererGL::~ImageRendererGL()
{
	ES_SAFE_DELETE(shader);
	ES_SAFE_DELETE(shader_no_texture);
	ES_SAFE_DELETE(vao);
	ES_SAFE_DELETE(vao_nt);
	ES_SAFE_DELETE(vertexBuffer);
}

void ImageRendererGL::Draw(const Effekseer::Vector3D positions[],
						   const Effekseer::Vector2D uvs[],
						   const Effekseer::Color colors[],
						   ::Effekseer::TextureRef texturePtr)
{
	Sprite s;

	for (int32_t i = 0; i < 4; i++)
	{
		s.Verteies[i].Pos = positions[i];
		s.Verteies[i].UV[0] = uvs[i].X;
		s.Verteies[i].UV[1] = uvs[i].Y;
		s.Verteies[i].SetColor(colors[i], false);
	}

	s.TexturePtr = texturePtr;

	sprites.push_back(s);
}

void ImageRendererGL::Render()
{
	if (sprites.size() == 0)
		return;

	for (int32_t i = 0; i < sprites.size(); i++)

		for (auto i = 0; i < sprites.size(); i++)
		{
			vertexBuffer->Lock();

			auto verteies = (EffekseerRendererGL::Vertex*)vertexBuffer->GetBufferDirect(sizeof(EffekseerRendererGL::Vertex) * 4);

			verteies[0] = sprites[i].Verteies[0];
			verteies[1] = sprites[i].Verteies[1];
			verteies[2] = sprites[i].Verteies[2];
			verteies[3] = sprites[i].Verteies[3];

			vertexBuffer->Unlock();

			auto& state = renderer->GetRenderState()->Push();
			state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
			state.DepthWrite = false;
			state.DepthTest = false;
			state.CullingType = Effekseer::CullingType::Double;
			state.TextureFilterTypes[0] = Effekseer::TextureFilterType::Nearest;
			state.TextureWrapTypes[0] = Effekseer::TextureWrapType::Clamp;

			EffekseerRendererGL::Shader* shader_ = nullptr;
			EffekseerRendererGL::VertexArray* vao_ = nullptr;

			if (sprites[i].TexturePtr != nullptr)
			{
				shader_ = (EffekseerRendererGL::Shader*)shader;
				vao_ = vao;
			}
			else
			{
				shader_ = (EffekseerRendererGL::Shader*)shader_no_texture;
				vao_ = vao_nt;
			}

			renderer->SetRenderMode(Effekseer::RenderMode::Normal);
			renderer->SetVertexArray(vao_);
			renderer->BeginShader(shader_);

			Effekseer::Matrix44 mats[2];
			mats[0].Indentity();
			mats[1].Indentity();

			memcpy(shader_->GetVertexConstantBuffer(), mats, sizeof(Effekseer::Matrix44) * 2);

			shader_->SetConstantBuffer();

			if (sprites[i].TexturePtr != nullptr)
			{
				auto texture = sprites[i].TexturePtr->GetBackend();
				renderer->SetTextures(shader_, &texture, 1);
			}

			renderer->GetRenderState()->Update(true);

			renderer->SetVertexBuffer((EffekseerRendererGL::VertexBuffer*)vertexBuffer, sizeof(EffekseerRendererGL::Vertex));

			renderer->SetLayout(shader_);

			// EffekseerRendererGL::GLExt::glActiveTexture(GL_TEXTURE0);
			// glBindTexture(GL_TEXTURE_2D, (GLuint)(size_t)sprites[i].TexturePtr);

			renderer->SetIndexBuffer(renderer->GetIndexBuffer());

			GLsizei stride = GL_UNSIGNED_SHORT;
			if (renderer->GetIndexBuffer()->GetStride() == 4)
			{
				stride = GL_UNSIGNED_INT;
			}

			glDrawElements(GL_TRIANGLES, 6, stride, nullptr);

			renderer->EndShader(shader_);
			renderer->SetVertexArray(nullptr);

			renderer->GetRenderState()->Pop();

			GLCheckError();
		}
}

void ImageRendererGL::ClearCache()
{
	sprites.clear();
}

void ImageRendererGL::OnLostDevice()
{
}

void ImageRendererGL::OnResetDevice()
{
}
} // namespace efk

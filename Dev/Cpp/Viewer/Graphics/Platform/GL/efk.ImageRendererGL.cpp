
#include "efk.ImageRendererGL.h"

#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.GLExtension.h>

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

static const char g_sprite_fs_texture_src[] =
	"IN lowp vec4 vaColor;\n"
	"IN mediump vec4 vaTexCoord;\n"

	"uniform sampler2D uTexture0;\n"

	"void main() {\n"
	"FRAGCOLOR = vaColor * TEX2D(uTexture0, vaTexCoord.xy);\n"
	"}\n";

static const char g_sprite_fs_no_texture_src[] =
	"IN lowp vec4 vaColor;\n"
	"IN mediump vec4 vaTexCoord;\n"

	"void main() {\n"
	"FRAGCOLOR = vaColor;\n"
	"}\n";



	ImageRendererGL::ImageRendererGL(EffekseerRenderer::Renderer* renderer)
		: ImageRenderer(renderer)
	{
		this->renderer = (EffekseerRendererGL::RendererImplemented*)renderer;

		auto shader_ = EffekseerRendererGL::Shader::Create(
			this->renderer,
			g_sprite_vs_src, sizeof(g_sprite_vs_src),
			g_sprite_fs_texture_src, sizeof(g_sprite_fs_texture_src),
			"Standard Tex");

		auto shader_no_texture_ = EffekseerRendererGL::Shader::Create(
			this->renderer,
			g_sprite_vs_src, sizeof(g_sprite_vs_src),
			g_sprite_fs_no_texture_src, sizeof(g_sprite_fs_no_texture_src),
			"Standard NoTex");


		EffekseerRendererGL::ShaderAttribInfo sprite_attribs[3] = {
			{ "atPosition", GL_FLOAT, 3, 0, false },
			{ "atColor", GL_UNSIGNED_BYTE, 4, 12, true },
			{ "atTexCoord", GL_FLOAT, 2, 16, false }
		};


		shader_->GetAttribIdList(3, sprite_attribs);
		shader_->SetVertexSize(sizeof(EffekseerRendererGL::Vertex));
		shader_->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);

		shader_->AddVertexConstantLayout(
			EffekseerRendererGL::CONSTANT_TYPE_MATRIX44,
			shader_->GetUniformId("uMatCamera"),
			0
		);

		shader_->AddVertexConstantLayout(
			EffekseerRendererGL::CONSTANT_TYPE_MATRIX44,
			shader_->GetUniformId("uMatProjection"),
			sizeof(Effekseer::Matrix44)
		);

		shader_->SetTextureSlot(0, shader_->GetUniformId("uTexture0"));

		shader_no_texture_->GetAttribIdList(3, sprite_attribs);
		shader_no_texture_->SetVertexSize(sizeof(EffekseerRendererGL::Vertex));
		shader_no_texture_->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);

		shader_no_texture_->AddVertexConstantLayout(
			EffekseerRendererGL::CONSTANT_TYPE_MATRIX44,
			shader_no_texture_->GetUniformId("uMatCamera"),
			0
		);

		shader_no_texture_->AddVertexConstantLayout(
			EffekseerRendererGL::CONSTANT_TYPE_MATRIX44,
			shader_no_texture_->GetUniformId("uMatProjection"),
			sizeof(Effekseer::Matrix44)
		);

		this->shader = shader_;
		this->shader_no_texture = shader_no_texture_;
        
		vertexBuffer = EffekseerRendererGL::VertexBuffer::Create(this->renderer, sizeof(EffekseerRendererGL::Vertex) * 12, true);

        vao = EffekseerRendererGL::VertexArray::Create(
                                                       this->renderer,shader_,
														(EffekseerRendererGL::VertexBuffer*)vertexBuffer,
                                                       this->renderer->GetIndexBuffer());
        vao_nt = EffekseerRendererGL::VertexArray::Create(
                                                       this->renderer,shader_no_texture_,
														(EffekseerRendererGL::VertexBuffer*)vertexBuffer,
                                                       this->renderer->GetIndexBuffer());

	}

	ImageRendererGL::~ImageRendererGL()
	{
		ES_SAFE_DELETE(shader);
		ES_SAFE_DELETE(shader_no_texture);
        ES_SAFE_DELETE(vao);
        ES_SAFE_DELETE(vao_nt);
		ES_SAFE_DELETE(vertexBuffer);
	}

	void ImageRendererGL::Draw(const Effekseer::Vector3D positions[], const Effekseer::Vector2D uvs[], const Effekseer::Color colors[], void* texturePtr)
	{
		Sprite s;

		for (int32_t i = 0; i < 4; i++)
		{
			s.Verteies[i].Pos = positions[i];
			s.Verteies[i].UV[0] = uvs[i].X;
			s.Verteies[i].UV[1] = uvs[i].Y;
			s.Verteies[i].Col = colors[i];
		}

		s.TexturePtr = texturePtr;

		sprites.push_back(s);
	}

	void ImageRendererGL::Render()
	{
		if (sprites.size() == 0) return;

		for(int32_t i = 0; i < sprites.size(); i++)

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

			renderer->GetRenderState()->Update(true);

			renderer->SetVertexBuffer((EffekseerRendererGL::VertexBuffer*)vertexBuffer, sizeof(EffekseerRendererGL::Vertex));

			renderer->SetLayout(shader_);

			EffekseerRendererGL::GLExt::glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, (GLuint)(size_t)sprites[i].TexturePtr);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

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
}

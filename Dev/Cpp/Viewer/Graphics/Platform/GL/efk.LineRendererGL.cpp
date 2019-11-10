
#include "efk.LineRendererGL.h"

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


	LineRendererGL::LineRendererGL(EffekseerRenderer::Renderer* renderer)
		: LineRenderer(renderer)
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

		this->shader = shader_no_texture_;
		ES_SAFE_DELETE(shader_);

		vertexBuffer = EffekseerRendererGL::VertexBuffer::Create(this->renderer, sizeof(EffekseerRendererGL::Vertex) * 1024, true, true);
        
        vao = EffekseerRendererGL::VertexArray::Create(
               this->renderer,shader_no_texture_,
               (EffekseerRendererGL::VertexBuffer*)vertexBuffer,
               nullptr, true);
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
		v0.SetColor(c);

		EffekseerRendererGL::Vertex v1;
		v1.Pos = p2;
		v1.SetColor(c);

		vertexies.push_back(v0);
		vertexies.push_back(v1);
	}

	void LineRendererGL::Render()
	{
		GLCheckError();
	
		if (vertexies.size() == 0) return;

		vertexBuffer->Lock();

		auto vs = (EffekseerRendererGL::Vertex*)vertexBuffer->GetBufferDirect(sizeof(EffekseerRendererGL::Vertex) * vertexies.size());
		if (vs == nullptr) return;

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
		
		Effekseer::TextureData* textures[2];
		textures[0] = nullptr;
		textures[1] = nullptr;

		renderer->SetTextures((EffekseerRendererGL::Shader*)shader, textures, 1);

		Effekseer::Matrix44 constantVSBuffer[2];
		constantVSBuffer[0] = renderer->GetCameraMatrix();
		constantVSBuffer[1] = renderer->GetProjectionMatrix();
		renderer->SetVertexBufferToShader(constantVSBuffer, sizeof(Effekseer::Matrix44) * 2, 0);

		shader->SetConstantBuffer();

		renderer->GetRenderState()->Update(false);

		renderer->SetVertexBuffer((EffekseerRendererGL::VertexBuffer*)vertexBuffer, sizeof(EffekseerRendererGL::Vertex));
		//EffekseerRendererGL::GLExt::glBindBuffer(GL_ARRAY_BUFFER, renderer->GetVertexBuffer()->GetInterface());
		//EffekseerRendererGL::GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
}

#if _WIN32
#define NOMINMAX
#endif

#include <algorithm>
#include "efk.PostEffectsGL.h"

#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.GLExtension.h>

namespace efk
{
	static const char g_basic_vs_src[] =
R"(
IN vec4 a_Position;
IN vec2 a_TexCoord;
OUT vec2 v_TexCoord;
void main() {
	gl_Position = a_Position;
	v_TexCoord = a_TexCoord.xy;
}
)";

static const char g_copy_fs_src[] =
R"(
IN vec2 v_TexCoord;
uniform sampler2D u_Texture0;
void main() {
	vec4 color = TEX2D(u_Texture0, v_TexCoord);
	FRAGCOLOR = vec4(color.xyz, 1.0);
}
)";

static const char g_extract_fs_src[] =
R"(
IN vec2 v_TexCoord;
uniform sampler2D u_Texture0;
uniform vec4 u_FilterParams;
uniform vec4 u_Intensity;
void main() {
	vec3 color = TEX2D(u_Texture0, v_TexCoord).rgb;
	float brightness = dot(color, vec3(0.299, 0.587, 0.114));
	float soft = brightness - u_FilterParams.y;
	soft = clamp(soft, 0.0, u_FilterParams.z);
	soft = soft * soft * u_FilterParams.w;
	float contribution = max(soft, brightness - u_FilterParams.x);
	contribution /= max(brightness, 0.00001);
	FRAGCOLOR = vec4(color.rgb * contribution * u_Intensity.x, 1.0);
}
)";

static const char g_blend_fs_src[] =
R"(
IN vec2 v_TexCoord;
uniform sampler2D u_Texture0;
uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;
uniform sampler2D u_Texture3;
const vec4 gain = vec4(0.25, 0.5, 1.0, 2.0);
void main() {
	vec3 c0 = TEX2D(u_Texture0, v_TexCoord).rgb * gain.x;
	vec3 c1 = TEX2D(u_Texture1, v_TexCoord).rgb * gain.y;
	vec3 c2 = TEX2D(u_Texture2, v_TexCoord).rgb * gain.z;
	vec3 c3 = TEX2D(u_Texture3, v_TexCoord).rgb * gain.w;
	FRAGCOLOR = vec4(c0 + c1 + c2 + c3, 1.0);
}
)";

static const char g_blur_h_fs_src[] =
R"(
IN vec2 v_TexCoord;
uniform sampler2D u_Texture0;
void main() {
	vec4 color = TEX2D    (u_Texture0, v_TexCoord              ) * 0.312500;
	color += textureOffset(u_Texture0, v_TexCoord, ivec2(-3, 0)) * 0.015625;
	color += textureOffset(u_Texture0, v_TexCoord, ivec2(-2, 0)) * 0.093750;
	color += textureOffset(u_Texture0, v_TexCoord, ivec2(-1, 0)) * 0.234375;
	color += textureOffset(u_Texture0, v_TexCoord, ivec2( 1, 0)) * 0.234375;
	color += textureOffset(u_Texture0, v_TexCoord, ivec2( 2, 0)) * 0.093750;
	color += textureOffset(u_Texture0, v_TexCoord, ivec2( 3, 0)) * 0.015625;
	FRAGCOLOR = vec4(color.rgb, 1.0);
}
)";

static const char g_blur_v_fs_src[] =
R"(
IN vec2 v_TexCoord;
uniform lowp sampler2D u_Texture0;
void main() {
	vec4 color = TEX2D    (u_Texture0, v_TexCoord              ) * 0.312500;
	color += textureOffset(u_Texture0, v_TexCoord, ivec2(0, -3)) * 0.015625;
	color += textureOffset(u_Texture0, v_TexCoord, ivec2(0, -2)) * 0.093750;
	color += textureOffset(u_Texture0, v_TexCoord, ivec2(0, -1)) * 0.234375;
	color += textureOffset(u_Texture0, v_TexCoord, ivec2(0,  1)) * 0.234375;
	color += textureOffset(u_Texture0, v_TexCoord, ivec2(0,  2)) * 0.093750;
	color += textureOffset(u_Texture0, v_TexCoord, ivec2(0,  3)) * 0.015625;
	FRAGCOLOR = vec4(color.rgb, 1.0);
}
)";

	// TextureFilter‚ðã‘‚«‚·‚é
	static void SetTexture(EffekseerRendererGL::Shader* shader, int32_t slot, GLuint texture)
	{
		using namespace EffekseerRendererGL;

		GLExt::glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		GLExt::glUniform1i(shader->GetTextureSlot(slot), slot);
	}

	BloomEffectGL::BloomEffectGL(Graphics* graphics)
		: BloomEffect(graphics)
	{
		using namespace Effekseer;
		using namespace EffekseerRendererGL;

		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		const ShaderAttribInfo shaderAttributes[2] = {
			{ "a_Position", GL_FLOAT, 2,  0, false },
			{ "a_TexCoord", GL_FLOAT, 2,  8, false }
		};

		// Extract shader
		shaderExtract.reset(Shader::Create(renderer,
			g_basic_vs_src, sizeof(g_basic_vs_src),
			g_extract_fs_src, sizeof(g_extract_fs_src),
			"Bloom extract"));
		shaderExtract->GetAttribIdList(2, shaderAttributes);
		shaderExtract->SetVertexSize(sizeof(Vertex));
		shaderExtract->SetTextureSlot(0, shaderExtract->GetUniformId("u_Texture0"));
		shaderExtract->SetPixelConstantBufferSize(sizeof(float) * 8);
		shaderExtract->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, 
			shaderExtract->GetUniformId("u_FilterParams"), 0);
		shaderExtract->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, 
			shaderExtract->GetUniformId("u_Intensity"), 0);

		// Copy shader
		shaderCopy.reset(Shader::Create(renderer,
			g_basic_vs_src, sizeof(g_basic_vs_src),
			g_copy_fs_src, sizeof(g_copy_fs_src),
			"Bloom copy"));
		shaderCopy->GetAttribIdList(2, shaderAttributes);
		shaderCopy->SetVertexSize(sizeof(Vertex));
		shaderCopy->SetTextureSlot(0, shaderCopy->GetUniformId("u_Texture0"));

		// Blend shader
		shaderBlend.reset(Shader::Create(renderer,
			g_basic_vs_src, sizeof(g_basic_vs_src),
			g_blend_fs_src, sizeof(g_blend_fs_src),
			"Bloom blend"));
		shaderBlend->GetAttribIdList(2, shaderAttributes);
		shaderBlend->SetVertexSize(sizeof(Vertex));
		shaderBlend->SetTextureSlot(0, shaderBlend->GetUniformId("u_Texture0"));
		shaderBlend->SetTextureSlot(1, shaderBlend->GetUniformId("u_Texture1"));
		shaderBlend->SetTextureSlot(2, shaderBlend->GetUniformId("u_Texture2"));
		shaderBlend->SetTextureSlot(3, shaderBlend->GetUniformId("u_Texture3"));

		// Blur(horizontal) shader
		shaderBlurH.reset(Shader::Create(renderer,
			g_basic_vs_src, sizeof(g_basic_vs_src),
			g_blur_h_fs_src, sizeof(g_blur_h_fs_src),
			"Bloom blurH"));
		shaderBlurH->GetAttribIdList(2, shaderAttributes);
		shaderBlurH->SetVertexSize(sizeof(Vertex));
		shaderBlurH->SetTextureSlot(0, shaderBlurH->GetUniformId("u_Texture0"));

		// Blur(vertical) shader
		shaderBlurV.reset(Shader::Create(renderer,
			g_basic_vs_src, sizeof(g_basic_vs_src),
			g_blur_v_fs_src, sizeof(g_blur_v_fs_src),
			"Bloom blurV"));
		shaderBlurV->GetAttribIdList(2, shaderAttributes);
		shaderBlurV->SetVertexSize(sizeof(Vertex));
		shaderBlurV->SetTextureSlot(0, shaderBlurV->GetUniformId("u_Texture0"));

		// Generate vertex data
		vertexBuffer.reset(VertexBuffer::Create(renderer, 
			sizeof(Vertex) * 4, true));
		vertexBuffer->Lock(); {
			Vertex* verteces = (Vertex*)vertexBuffer->GetBufferDirect(sizeof(Vertex) * 4);
			verteces[0] = Vertex{-1.0f,  1.0f, 0.0f, 1.0f};
			verteces[1] = Vertex{-1.0f, -1.0f, 0.0f, 0.0f};
			verteces[2] = Vertex{ 1.0f,  1.0f, 1.0f, 1.0f};
			verteces[3] = Vertex{ 1.0f, -1.0f, 1.0f, 0.0f};
		}
		vertexBuffer->Unlock();

		// Setup VAOs
		vaoExtract.reset(VertexArray::Create(renderer, 
			shaderExtract.get(), vertexBuffer.get(), renderer->GetIndexBuffer()));
		vaoCopy.reset(VertexArray::Create(renderer, 
			shaderCopy.get(), vertexBuffer.get(), renderer->GetIndexBuffer()));
		vaoBlend.reset(VertexArray::Create(renderer, 
			shaderBlend.get(), vertexBuffer.get(), renderer->GetIndexBuffer()));
		vaoBlurH.reset(VertexArray::Create(renderer, 
			shaderBlurH.get(), vertexBuffer.get(), renderer->GetIndexBuffer()));
		vaoBlurV.reset(VertexArray::Create(renderer, 
			shaderBlurV.get(), vertexBuffer.get(), renderer->GetIndexBuffer()));
	}

	BloomEffectGL::~BloomEffectGL()
	{
	}

	void BloomEffectGL::Render(RenderTexture* src, RenderTexture* dest)
	{
		if( !enabled )
		{
			return;
		}

		using namespace Effekseer;
		using namespace EffekseerRendererGL;

		auto renderer = (RendererImplemented*)graphics->GetRenderer();
		auto renderTexture = graphics->GetRenderTexture();
		auto depthTexture = graphics->GetDepthTexture();

		if (renderTextureWidth  != renderTexture->GetWidth() || 
			renderTextureHeight != renderTexture->GetHeight())
		{
			SetupBuffers(renderTexture->GetWidth(), renderTexture->GetHeight());
		}

		auto& state = renderer->GetRenderState()->Push();
		state.AlphaBlend = AlphaBlendType::Opacity;
		state.DepthWrite = false;
		state.DepthTest = false;
		state.CullingType = CullingType::Double;
		renderer->GetRenderState()->Update(false);
		renderer->SetRenderMode(RenderMode::Normal);

		// Extract pass
		renderer->SetVertexArray(vaoExtract.get());
		renderer->BeginShader(shaderExtract.get());
		{
			const float knee = threshold * softKnee;
			const float constantData[8] = {
				threshold, 
				threshold - knee, 
				knee * 2.0f, 
				0.25f / (knee + 0.00001f),
				intensity,
			};
			memcpy(shaderExtract->GetPixelConstantBuffer(), constantData, sizeof(constantData));
			shaderExtract->SetConstantBuffer();
			graphics->SetRenderTarget(extractBuffer.get(), nullptr);
			SetTexture(shaderExtract.get(), 0, (GLuint)renderTexture->GetViewID());
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
		}
		renderer->EndShader(shaderExtract.get());

		// Shrink pass
		renderer->SetVertexArray(vaoCopy.get());
		renderer->BeginShader(shaderCopy.get());
		for (int i = 0; i < BlurIterations; i++)
		{
			graphics->SetRenderTarget(lowresBuffers[0][i].get(), nullptr);
			SetTexture(shaderCopy.get(), 0, (GLuint)extractBuffer->GetViewID());
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
		}
		renderer->EndShader(shaderCopy.get());

		// Horizontal gaussian blur pass
		renderer->SetVertexArray(vaoBlurH.get());
		renderer->BeginShader(shaderBlurH.get());
		for (int i = 0; i < BlurIterations; i++)
		{
			graphics->SetRenderTarget(lowresBuffers[1][i].get(), nullptr);
			SetTexture(shaderBlurH.get(), 0, (GLuint)lowresBuffers[0][i]->GetViewID());
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
		}
		renderer->EndShader(shaderBlurH.get());

		// Vertical gaussian blur pass
		renderer->SetVertexArray(vaoBlurV.get());
		renderer->BeginShader(shaderBlurV.get());
		for (int i = 0; i < BlurIterations; i++)
		{
			graphics->SetRenderTarget(lowresBuffers[0][i].get(), nullptr);
			SetTexture(shaderBlurV.get(), 0, (GLuint)lowresBuffers[1][i]->GetViewID());
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
		}
		renderer->EndShader(shaderBlurV.get());

		// Blending pass
		state.AlphaBlend = AlphaBlendType::Add;
		renderer->GetRenderState()->Update(false);
		renderer->SetVertexArray(vaoBlend.get());
		renderer->BeginShader(shaderBlend.get());
		{
			graphics->SetRenderTarget(renderTexture, depthTexture);
			SetTexture(shaderBlend.get(), 0, (GLuint)lowresBuffers[0][0]->GetViewID());
			SetTexture(shaderBlend.get(), 1, (GLuint)lowresBuffers[0][1]->GetViewID());
			SetTexture(shaderBlend.get(), 2, (GLuint)lowresBuffers[0][2]->GetViewID());
			SetTexture(shaderBlend.get(), 3, (GLuint)lowresBuffers[0][3]->GetViewID());
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
		}
		renderer->EndShader(shaderBlend.get());

		GLExt::glActiveTexture(GL_TEXTURE0);
		renderer->GetRenderState()->Update(true);
		renderer->GetRenderState()->Pop();
		GLCheckError();
	}

	void BloomEffectGL::OnLostDevice()
	{
		ReleaseBuffers();
	}

	void BloomEffectGL::OnResetDevice()
	{
	}

	void BloomEffectGL::SetupBuffers(int32_t width, int32_t height)
	{
		ReleaseBuffers();

		renderTextureWidth = width;
		renderTextureHeight = height;

		const int32_t divides[BlurIterations] = {4, 8, 16, 32};

		// Create high brightness extraction buffer
		{
			int32_t bufferWidth  = std::max(1, width  / 2);
			int32_t bufferHeight = std::max(1, height / 2);
			extractBuffer.reset(RenderTexture::Create(graphics));
			extractBuffer->Initialize(bufferWidth, bufferHeight, TextureFormat::RGBA16F);
		}

		// Create low-resolution buffers
		for (int i = 0; i < BlurBuffers; i++) {
			for (int j = 0; j < BlurIterations; j++) {
				int32_t bufferWidth  = std::max(1, width  / divides[j]);
				int32_t bufferHeight = std::max(1, height / divides[j]);
				lowresBuffers[i][j].reset(RenderTexture::Create(graphics));
				lowresBuffers[i][j]->Initialize(bufferWidth, bufferHeight, TextureFormat::RGBA16F);
			}
		}
	}

	void BloomEffectGL::ReleaseBuffers()
	{
		renderTextureWidth = 0;
		renderTextureHeight = 0;

		extractBuffer.reset();
		for (int i = 0; i < BlurBuffers; i++) {
			for (int j = 0; j < BlurIterations; j++) {
				lowresBuffers[i][j].reset();
			}
		}
	}
}

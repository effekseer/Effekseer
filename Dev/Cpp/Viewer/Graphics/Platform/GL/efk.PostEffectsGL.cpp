#if _WIN32
#define NOMINMAX
#endif

#include <algorithm>
#include <iostream>

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

static const char g_downsample_fs_src[] =
R"(
IN vec2 v_TexCoord;
uniform sampler2D u_Texture0;
const vec4 gain = vec4(0.25, 0.5, 1.0, 2.0);
void main() {
	ivec2 size = textureSize(u_Texture0, 0);
	vec2 scale = vec2(1.0 / size.x, 1.0 / size.y);
	vec4 c0 = TEX2D(u_Texture0, v_TexCoord + vec2(-0.5, -0.5) * scale);
	vec4 c1 = TEX2D(u_Texture0, v_TexCoord + vec2(+0.5, -0.5) * scale);
	vec4 c2 = TEX2D(u_Texture0, v_TexCoord + vec2(-0.5, +0.5) * scale);
	vec4 c3 = TEX2D(u_Texture0, v_TexCoord + vec2(+0.5, +0.5) * scale);
	FRAGCOLOR = (c0 + c1 + c2 + c3) * 0.25;
}
)";


static const char g_blend_fs_src[] =
R"(
IN vec2 v_TexCoord;
uniform sampler2D u_Texture0;
uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;
uniform sampler2D u_Texture3;
void main() {
	vec3 c0 = TEX2D(u_Texture0, v_TexCoord).rgb;
	vec3 c1 = TEX2D(u_Texture1, v_TexCoord).rgb;
	vec3 c2 = TEX2D(u_Texture2, v_TexCoord).rgb;
	vec3 c3 = TEX2D(u_Texture3, v_TexCoord).rgb;
	FRAGCOLOR = vec4(c0 + c1 + c2 + c3, 1.0);
}
)";



static const char g_blur_h_fs_src[] =
R"(
IN vec2 v_TexCoord;
uniform sampler2D u_Texture0;
void main() {

	ivec2 size = textureSize(u_Texture0, 0);
	float div = float(size.x);

	vec4 color = TEX2D(u_Texture0, v_TexCoord) * 0.223067435;
	color += TEX2D(u_Texture0, v_TexCoord + vec2(-5.152032242 / div, 0)) * 0.005291685;
	color += TEX2D(u_Texture0, v_TexCoord + vec2(-3.250912787 / div, 0)) * 0.072975516;
	color += TEX2D(u_Texture0, v_TexCoord + vec2(-1.384912144 / div, 0)) * 0.310199082;
	color += TEX2D(u_Texture0, v_TexCoord + vec2(+1.384912144 / div, 0)) * 0.310199082;
	color += TEX2D(u_Texture0, v_TexCoord + vec2(+3.250912787 / div, 0)) * 0.072975516;
	color += TEX2D(u_Texture0, v_TexCoord + vec2(+5.152032242 / div, 0)) * 0.005291685;

	FRAGCOLOR = vec4(color.rgb, 1.0);
}
)";

static const char g_blur_v_fs_src[] =
R"(
IN vec2 v_TexCoord;
uniform sampler2D u_Texture0;
void main() {

	ivec2 size = textureSize(u_Texture0, 0);
	float div = float(size.y);

	vec4 color = TEX2D(u_Texture0, v_TexCoord) * 0.223067435;
	color += TEX2D(u_Texture0, v_TexCoord + vec2(0.0, -5.152032242 / div)) * 0.005291685;
	color += TEX2D(u_Texture0, v_TexCoord + vec2(0.0, -3.250912787 / div)) * 0.072975516;
	color += TEX2D(u_Texture0, v_TexCoord + vec2(0.0, -1.384912144 / div)) * 0.310199082;
	color += TEX2D(u_Texture0, v_TexCoord + vec2(0.0, +1.384912144 / div)) * 0.310199082;
	color += TEX2D(u_Texture0, v_TexCoord + vec2(0.0, +3.250912787 / div)) * 0.072975516;
	color += TEX2D(u_Texture0, v_TexCoord + vec2(0.0, +5.152032242 / div)) * 0.005291685;

	FRAGCOLOR = vec4(color.rgb, 1.0);
}
)";

static const char g_tonemap_reinhard_fs_src[] =
R"(
IN mediump vec2 v_TexCoord;
uniform sampler2D u_Texture0;
uniform vec4 u_Exposure;
void main() {
	vec3 color = texture(u_Texture0, v_TexCoord).rgb;
	float lum = u_Exposure.x * dot(color, vec3(0.299, 0.587, 0.114));
	lum = lum / (1.0 + lum);
	FRAGCOLOR = vec4(lum * color, 1.0f);
}
)";

static const char g_linear_to_srgb_fs_src[] =
	R"(
IN vec2 v_TexCoord;
uniform sampler2D u_Texture0;
void main() {
	vec4 color = TEX2D(u_Texture0, v_TexCoord);
	FRAGCOLOR = vec4(pow(color.xyz, vec3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2)), 1.0);
}
)";

	const EffekseerRendererGL::ShaderAttribInfo BlitterGL::shaderAttributes[2] = {
		{ "a_Position", GL_FLOAT, 2,  0, false },
		{ "a_TexCoord", GL_FLOAT, 2,  8, false }
	};

	BlitterGL::BlitterGL(Graphics* graphics)
		: graphics(graphics)
	{
		using namespace EffekseerRendererGL;
		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		// Generate vertex data
		vertexBuffer.reset(VertexBuffer::Create(renderer, sizeof(Vertex) * 4, true, true));

		vertexBuffer->Lock();
		{
			Vertex* verteces = (Vertex*)vertexBuffer->GetBufferDirect(sizeof(Vertex) * 4);
			verteces[0] = Vertex{-1.0f,  1.0f, 0.0f, 1.0f};
			verteces[1] = Vertex{-1.0f, -1.0f, 0.0f, 0.0f};
			verteces[2] = Vertex{ 1.0f,  1.0f, 1.0f, 1.0f};
			verteces[3] = Vertex{ 1.0f, -1.0f, 1.0f, 0.0f};
		}
		vertexBuffer->Unlock();
	}

	BlitterGL::~BlitterGL()
	{
	}

	std::unique_ptr<EffekseerRendererGL::VertexArray>
		BlitterGL::CreateVAO(EffekseerRendererGL::Shader* shader)
	{
		using namespace EffekseerRendererGL;
		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		return std::unique_ptr<VertexArray>(VertexArray::Create(renderer, shader, vertexBuffer.get(), renderer->GetIndexBuffer(), true));
	}

	void BlitterGL::Blit(EffekseerRendererGL::Shader* shader, EffekseerRendererGL::VertexArray* vao,
		int32_t numTextures, const GLuint* textures, 
		const void* constantData, size_t constantDataSize, RenderTexture* dest)
	{
		using namespace Effekseer;
		using namespace EffekseerRendererGL;
		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		// Set GLStates
		renderer->SetVertexArray(vao);
		renderer->BeginShader(shader);
		
		// Set shader parameters
		if (constantData != nullptr) {
			memcpy(shader->GetPixelConstantBuffer(), constantData, constantDataSize);
			shader->SetConstantBuffer();
		}
		
		// Set destination texture
		graphics->SetRenderTarget(dest, nullptr);

		// Set source textures
		for (int32_t slot = 0; slot < numTextures; slot++) {
			GLExt::glBindSampler(slot, 0);
			GLExt::glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, textures[slot]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			GLExt::glUniform1i(shader->GetTextureSlot(slot), slot);

			GLCheckError();
		}
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
		GLCheckError();

		renderer->EndShader(shader);
	}

	BloomEffectGL::BloomEffectGL(Graphics* graphics)
		: BloomEffect(graphics), blitter(graphics)
	{
		using namespace EffekseerRendererGL;
		auto renderer = (RendererImplemented*)graphics->GetRenderer();
		
		// Extract shader
		shaderExtract.reset(Shader::Create(renderer,
			g_basic_vs_src, sizeof(g_basic_vs_src),
			g_extract_fs_src, sizeof(g_extract_fs_src),
			"Bloom extract"));
		shaderExtract->GetAttribIdList(2, BlitterGL::shaderAttributes);
		shaderExtract->SetVertexSize(sizeof(BlitterGL::Vertex));
		shaderExtract->SetTextureSlot(0, shaderExtract->GetUniformId("u_Texture0"));
		shaderExtract->SetPixelConstantBufferSize(sizeof(float) * 8);
		shaderExtract->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, 
			shaderExtract->GetUniformId("u_FilterParams"), 0);
		shaderExtract->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, 
			shaderExtract->GetUniformId("u_Intensity"), 16);

		// Downsample shader
		shaderDownsample.reset(Shader::Create(renderer,
			g_basic_vs_src, sizeof(g_basic_vs_src),
			g_downsample_fs_src, sizeof(g_downsample_fs_src),
			"Bloom downsample"));
		shaderDownsample->GetAttribIdList(2, BlitterGL::shaderAttributes);
		shaderDownsample->SetVertexSize(sizeof(BlitterGL::Vertex));
		shaderDownsample->SetTextureSlot(0, shaderDownsample->GetUniformId("u_Texture0"));

		// Blend shader
		shaderBlend.reset(Shader::Create(renderer,
			g_basic_vs_src, sizeof(g_basic_vs_src),
			g_blend_fs_src, sizeof(g_blend_fs_src),
			"Bloom blend"));
		shaderBlend->GetAttribIdList(2, BlitterGL::shaderAttributes);
		shaderBlend->SetVertexSize(sizeof(BlitterGL::Vertex));
		shaderBlend->SetTextureSlot(0, shaderBlend->GetUniformId("u_Texture0"));
		shaderBlend->SetTextureSlot(1, shaderBlend->GetUniformId("u_Texture1"));
		shaderBlend->SetTextureSlot(2, shaderBlend->GetUniformId("u_Texture2"));
		shaderBlend->SetTextureSlot(3, shaderBlend->GetUniformId("u_Texture3"));

		// Blur(horizontal) shader
		shaderBlurH.reset(Shader::Create(renderer,
			g_basic_vs_src, sizeof(g_basic_vs_src),
			g_blur_h_fs_src, sizeof(g_blur_h_fs_src),
			"Bloom blurH"));
		shaderBlurH->GetAttribIdList(2, BlitterGL::shaderAttributes);
		shaderBlurH->SetVertexSize(sizeof(BlitterGL::Vertex));
		shaderBlurH->SetTextureSlot(0, shaderBlurH->GetUniformId("u_Texture0"));

		// Blur(vertical) shader
		shaderBlurV.reset(Shader::Create(renderer,
			g_basic_vs_src, sizeof(g_basic_vs_src),
			g_blur_v_fs_src, sizeof(g_blur_v_fs_src),
			"Bloom blurV"));
		shaderBlurV->GetAttribIdList(2, BlitterGL::shaderAttributes);
		shaderBlurV->SetVertexSize(sizeof(BlitterGL::Vertex));
		shaderBlurV->SetTextureSlot(0, shaderBlurV->GetUniformId("u_Texture0"));

		// Setup VAOs
		vaoExtract = blitter.CreateVAO(shaderExtract.get());
		vaoDownsample = blitter.CreateVAO(shaderDownsample.get());
		vaoBlend = blitter.CreateVAO(shaderBlend.get());
		vaoBlurH = blitter.CreateVAO(shaderBlurH.get());
		vaoBlurV = blitter.CreateVAO(shaderBlurV.get());
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
        
		if (renderTextureWidth  != src->GetWidth() ||
			renderTextureHeight != src->GetHeight())
		{
			SetupBuffers(src->GetWidth(), src->GetHeight());
		}

		auto& state = renderer->GetRenderState()->Push();
		state.AlphaBlend = AlphaBlendType::Opacity;
		state.DepthWrite = false;
		state.DepthTest = false;
		state.CullingType = CullingType::Double;
		state.TextureFilterTypes[0] = TextureFilterType::Linear;
		state.TextureWrapTypes[0] = TextureWrapType::Clamp;
		renderer->GetRenderState()->Update(false);
		renderer->SetRenderMode(RenderMode::Normal);

		// Extract pass
		{
			const float knee = threshold * softKnee;
			const float constantData[8] = {
				threshold, 
				threshold - knee, 
				knee * 2.0f, 
				0.25f / (knee + 0.00001f),
				intensity,
			};
			const GLuint textures[] = {
				(GLuint)src->GetViewID()
			};
			blitter.Blit(shaderExtract.get(), vaoExtract.get(), 1, textures, 
				constantData, sizeof(constantData), extractBuffer.get());
		}

		// Shrink pass
		for (int i = 0; i < BlurIterations; i++)
		{
			GLuint textures[1];
			textures[0] = (i == 0) ?
				(GLuint)extractBuffer->GetViewID() : 
				(GLuint)lowresBuffers[0][i - 1]->GetViewID();
			blitter.Blit(shaderDownsample.get(), vaoDownsample.get(), 1, textures, 
				nullptr, 0, lowresBuffers[0][i].get());
		}

		// Horizontal gaussian blur pass
		for (int i = 0; i < BlurIterations; i++)
		{
			const GLuint textures[] = {
				(GLuint)lowresBuffers[0][i]->GetViewID()
			};
			blitter.Blit(shaderBlurH.get(), vaoBlurH.get(), 1, textures, 
				nullptr, 0, lowresBuffers[1][i].get());
		}

		// Vertical gaussian blur pass
		for (int i = 0; i < BlurIterations; i++)
		{
			const GLuint textures[] = {
				(GLuint)lowresBuffers[1][i]->GetViewID()
			};
			blitter.Blit(shaderBlurV.get(), vaoBlurV.get(), 1, textures, 
				nullptr, 0, lowresBuffers[0][i].get());
		}

		// Blending pass
		state.AlphaBlend = AlphaBlendType::Add;
		renderer->GetRenderState()->Update(false);
		{
			const GLuint textures[] = {
				(GLuint)lowresBuffers[0][0]->GetViewID(),
				(GLuint)lowresBuffers[0][1]->GetViewID(),
				(GLuint)lowresBuffers[0][2]->GetViewID(),
				(GLuint)lowresBuffers[0][3]->GetViewID()
			};
			blitter.Blit(shaderBlend.get(), vaoBlend.get(), 4, textures, 
				nullptr, 0, dest);
		}

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

		// Create high brightness extraction buffer
		{
			int32_t bufferWidth  = width;
			int32_t bufferHeight = height;
			extractBuffer.reset(RenderTexture::Create(graphics));
			extractBuffer->Initialize(bufferWidth, bufferHeight, TextureFormat::RGBA16F);
		}

		// Create low-resolution buffers
		for (int i = 0; i < BlurBuffers; i++) {
			int32_t bufferWidth  = width;
			int32_t bufferHeight = height;
			for (int j = 0; j < BlurIterations; j++) {
				bufferWidth  = std::max(1, (bufferWidth  + 1) / 2);
				bufferHeight = std::max(1, (bufferHeight + 1) / 2);
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

	TonemapEffectGL::TonemapEffectGL(Graphics* graphics) 
		: TonemapEffect(graphics), blitter(graphics)
	{
		using namespace EffekseerRendererGL;
		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		// Copy shader
		shaderCopy.reset(Shader::Create(renderer, 
			g_basic_vs_src, sizeof(g_basic_vs_src), 
			g_copy_fs_src, sizeof(g_copy_fs_src), 
			"Tonemap copy"));
		shaderCopy->GetAttribIdList(2, BlitterGL::shaderAttributes);
		shaderCopy->SetVertexSize(sizeof(BlitterGL::Vertex));
		shaderCopy->SetTextureSlot(0, shaderCopy->GetUniformId("u_Texture0"));

		// Reinhard shader
		shaderReinhard.reset(Shader::Create(renderer,
			g_basic_vs_src, sizeof(g_basic_vs_src), 
			g_tonemap_reinhard_fs_src, sizeof(g_tonemap_reinhard_fs_src), 
			"Tonemap Reinhard"));
		shaderReinhard->GetAttribIdList(2, BlitterGL::shaderAttributes);
		shaderReinhard->SetVertexSize(sizeof(BlitterGL::Vertex));
		shaderReinhard->SetTextureSlot(0, shaderReinhard->GetUniformId("u_Texture0"));
		shaderReinhard->SetPixelConstantBufferSize(sizeof(float) * 4);
		shaderReinhard->AddPixelConstantLayout(CONSTANT_TYPE_VECTOR4, 
			shaderReinhard->GetUniformId("u_Exposure"), 0);

		// Setup VAOs
		vaoCopy = blitter.CreateVAO(shaderCopy.get());
		vaoReinhard = blitter.CreateVAO(shaderReinhard.get());
	}

	TonemapEffectGL::~TonemapEffectGL()
	{
	}

	void TonemapEffectGL::Render(RenderTexture* src, RenderTexture* dest)
	{
		using namespace Effekseer;
		using namespace EffekseerRendererGL;
		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		auto& state = renderer->GetRenderState()->Push();
		state.AlphaBlend = AlphaBlendType::Opacity;
		state.DepthWrite = false;
		state.DepthTest = false;
		state.CullingType = CullingType::Double;
		renderer->GetRenderState()->Update(false);
		renderer->SetRenderMode(RenderMode::Normal);

		const GLuint textures[] = {
			(GLuint)src->GetViewID()
		};

		if (algorithm == Algorithm::Off) {
			blitter.Blit(shaderCopy.get(), vaoCopy.get(), 1, textures, nullptr, 0, dest);
		} else if (algorithm == Algorithm::Reinhard) {
			const float constantData[4] = {exposure, 16.0f * 16.0f};
			blitter.Blit(shaderReinhard.get(), vaoReinhard.get(), 1, textures, constantData, sizeof(constantData), dest);
		}

		GLExt::glActiveTexture(GL_TEXTURE0);
		renderer->GetRenderState()->Update(true);
		renderer->GetRenderState()->Pop();
		GLCheckError();
	}

	LinearToSRGBEffectGL::LinearToSRGBEffectGL(Graphics* graphics) : LinearToSRGBEffect(graphics), blitter(graphics)
	{
		using namespace EffekseerRendererGL;
		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		// Copy shader
		shader_.reset(Shader::Create(
			renderer, g_basic_vs_src, sizeof(g_basic_vs_src), g_linear_to_srgb_fs_src, sizeof(g_linear_to_srgb_fs_src), "LinearToSRGB"));
		shader_->GetAttribIdList(2, BlitterGL::shaderAttributes);
		shader_->SetVertexSize(sizeof(BlitterGL::Vertex));
		shader_->SetTextureSlot(0, shader_->GetUniformId("u_Texture0"));


		// Setup VAOs
		vao_ = blitter.CreateVAO(shader_.get());
	}

	LinearToSRGBEffectGL::~LinearToSRGBEffectGL() {}

	void LinearToSRGBEffectGL::Render(RenderTexture* src, RenderTexture* dest)
	{
		using namespace Effekseer;
		using namespace EffekseerRendererGL;
		auto renderer = (RendererImplemented*)graphics->GetRenderer();

		auto& state = renderer->GetRenderState()->Push();
		state.AlphaBlend = AlphaBlendType::Opacity;
		state.DepthWrite = false;
		state.DepthTest = false;
		state.CullingType = CullingType::Double;
		renderer->GetRenderState()->Update(false);
		renderer->SetRenderMode(RenderMode::Normal);

		const GLuint textures[] = {(GLuint)src->GetViewID()};

		blitter.Blit(shader_.get(), vao_.get(), 1, textures, nullptr, 0, dest);

		GLExt::glActiveTexture(GL_TEXTURE0);
		renderer->GetRenderState()->Update(true);
		renderer->GetRenderState()->Pop();
		GLCheckError();
	}
}


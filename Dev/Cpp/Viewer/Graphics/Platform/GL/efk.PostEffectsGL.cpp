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

const Effekseer::Backend::VertexLayoutElement vlElem[2] = {
	{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "a_Position", "POSITION", 0},
	{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "a_TexCoord", "TEXCOORD", 0},
};

BlitterGL::BlitterGL(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: graphics(graphics)
	, renderer_(EffekseerRendererGL::RendererImplementedRef::FromPinned(renderer.Get()))
{
	using namespace EffekseerRendererGL;

	auto gd = this->renderer_->GetGraphicsDevice().DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();

	// Generate vertex data
	vertexBuffer.reset(VertexBuffer::Create(gd, true, sizeof(Vertex) * 4, true));

	vertexBuffer->Lock();
	{
		Vertex* verteces = (Vertex*)vertexBuffer->GetBufferDirect(sizeof(Vertex) * 4);
		verteces[0] = Vertex{-1.0f, 1.0f, 0.0f, 1.0f};
		verteces[1] = Vertex{-1.0f, -1.0f, 0.0f, 0.0f};
		verteces[2] = Vertex{1.0f, 1.0f, 1.0f, 1.0f};
		verteces[3] = Vertex{1.0f, -1.0f, 1.0f, 0.0f};
	}
	vertexBuffer->Unlock();
}

BlitterGL::~BlitterGL()
{
}

std::unique_ptr<EffekseerRendererGL::VertexArray> BlitterGL::CreateVAO(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, EffekseerRendererGL::Shader* shader)
{
	using namespace EffekseerRendererGL;
	auto gd = graphicsDevice.DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();

	return std::unique_ptr<VertexArray>(VertexArray::Create(gd, shader, vertexBuffer.get(), renderer_->GetIndexBuffer()));
}

void BlitterGL::Blit(EffekseerRendererGL::Shader* shader,
					 EffekseerRendererGL::VertexArray* vao,
					 const std::vector<Effekseer::Backend::TextureRef>& textures,
					 const void* constantData,
					 size_t constantDataSize,
					 Effekseer::Backend::TextureRef dest,
					 bool isCleared)
{
	using namespace Effekseer;
	using namespace EffekseerRendererGL;

	// Set GLStates
	renderer_->SetVertexArray(vao);
	renderer_->BeginShader(shader);

	// Set shader parameters
	if (constantData != nullptr)
	{
		memcpy(shader->GetPixelConstantBuffer(), constantData, constantDataSize);
		shader->SetConstantBuffer();
	}

	// Set destination texture
	graphics->SetRenderTarget({dest}, nullptr);

	if (isCleared)
	{
		graphics->Clear(Effekseer::Color(0, 0, 0, 0));
	}

	// Set source textures
	for (size_t slot = 0; slot < textures.size(); slot++)
	{
		auto texture = textures[slot];
		auto t = texture.DownCast<EffekseerRendererGL::Backend::Texture>();
		GLExt::glBindSampler(slot, 0);
		GLExt::glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, t->GetBuffer());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		GLExt::glUniform1i(shader->GetTextureSlot(slot), slot);

		GLCheckError();
	}

	GLsizei stride = GL_UNSIGNED_SHORT;
	if (renderer_->GetIndexBuffer()->GetStride() == 4)
	{
		stride = GL_UNSIGNED_INT;
	}

	glDrawElements(GL_TRIANGLES, 6, stride, nullptr);
	GLCheckError();

	renderer_->EndShader(shader);
}

BloomEffectGL::BloomEffectGL(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: BloomEffect(graphics)
	, blitter(graphics, renderer)
	, renderer_(EffekseerRendererGL::RendererImplementedRef::FromPinned(renderer.Get()))
{
	auto vl = renderer->GetGraphicsDevice()->CreateVertexLayout(vlElem, 2).DownCast<EffekseerRendererGL::Backend::VertexLayout>();

	EffekseerRendererGL::ShaderCodeView basicVS(g_basic_vs_src);

	// Extract shader
	EffekseerRendererGL::ShaderCodeView extractPS(g_extract_fs_src);

	using namespace Effekseer::Backend;
	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElm;
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Pixel, "u_FilterParams", UniformBufferLayoutElementType::Vector4, 1, 0});
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Pixel, "u_Intensity", UniformBufferLayoutElementType::Vector4, 1, sizeof(float) * 4});
	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"u_Texture0"}, uniformLayoutElm);

	auto uniformLayoutSingleImage = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"u_Texture0"}, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>{});
	auto uniformLayoutImages = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"u_Texture0", "u_Texture1", "u_Texture2", "u_Texture3"}, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>{});

	shaderExtract.reset(EffekseerRendererGL::Shader::CreateWithHeader(renderer_->GetInternalGraphicsDevice(), basicVS, extractPS, uniformLayout, "Bloom extract"));

	shaderExtract->SetVertexLayout(vl);
	shaderExtract->SetPixelConstantBufferSize(sizeof(float) * 8);

	// Downsample shader

	EffekseerRendererGL::ShaderCodeView downSamplePS(g_downsample_fs_src);

	shaderDownsample.reset(EffekseerRendererGL::Shader::CreateWithHeader(renderer_->GetInternalGraphicsDevice(), basicVS, downSamplePS, uniformLayoutSingleImage, "Bloom downsample"));
	shaderDownsample->SetVertexLayout(vl);

	// Blend shader
	EffekseerRendererGL::ShaderCodeView blendPS(g_blend_fs_src);

	shaderBlend.reset(EffekseerRendererGL::Shader::CreateWithHeader(renderer_->GetInternalGraphicsDevice(), basicVS, blendPS, uniformLayoutImages, "Bloom blend"));
	shaderBlend->SetVertexLayout(vl);

	// Blur(horizontal) shader
	EffekseerRendererGL::ShaderCodeView blend_h_PS(g_blur_h_fs_src);

	shaderBlurH.reset(EffekseerRendererGL::Shader::CreateWithHeader(renderer_->GetInternalGraphicsDevice(), basicVS, blend_h_PS, uniformLayoutSingleImage, "Bloom blurH"));
	shaderBlurH->SetVertexLayout(vl);

	// Blur(vertical) shader
	EffekseerRendererGL::ShaderCodeView blend_v_PS(g_blur_v_fs_src);

	shaderBlurV.reset(EffekseerRendererGL::Shader::CreateWithHeader(renderer_->GetInternalGraphicsDevice(), basicVS, blend_v_PS, uniformLayoutSingleImage, "Bloom blurV"));
	shaderBlurV->SetVertexLayout(vl);

	// Setup VAOs
	vaoExtract = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderExtract.get());
	vaoDownsample = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderDownsample.get());
	vaoBlend = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderBlend.get());
	vaoBlurH = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderBlurH.get());
	vaoBlurV = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderBlurV.get());
}

BloomEffectGL::~BloomEffectGL()
{
}

void BloomEffectGL::Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest)
{
	if (!enabled)
	{
		return;
	}

	using namespace Effekseer;
	using namespace EffekseerRendererGL;

	if (renderTextureWidth != src->GetSize()[0] || renderTextureHeight != src->GetSize()[1])
	{
		SetupBuffers(src->GetSize()[0], src->GetSize()[1]);
	}

	auto& state = renderer_->GetRenderState()->Push();
	state.AlphaBlend = AlphaBlendType::Opacity;
	state.DepthWrite = false;
	state.DepthTest = false;
	state.CullingType = CullingType::Double;
	state.TextureFilterTypes[0] = TextureFilterType::Linear;
	state.TextureWrapTypes[0] = TextureWrapType::Clamp;
	renderer_->GetRenderState()->Update(false);
	renderer_->SetRenderMode(RenderMode::Normal);

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
		blitter.Blit(shaderExtract.get(), vaoExtract.get(), std::vector<Effekseer::Backend::TextureRef>{src}, constantData, sizeof(constantData), extractBuffer->GetAsBackend());
	}

	// Shrink pass
	for (int i = 0; i < BlurIterations; i++)
	{
		const auto textures = (i == 0) ? extractBuffer->GetAsBackend() : lowresBuffers[0][i - 1]->GetAsBackend();
		blitter.Blit(shaderDownsample.get(), vaoDownsample.get(), std::vector<Effekseer::Backend::TextureRef>{textures}, nullptr, 0, lowresBuffers[0][i]->GetAsBackend());
	}

	// Horizontal gaussian blur pass
	for (int i = 0; i < BlurIterations; i++)
	{
		const std::vector<Effekseer::Backend::TextureRef> textures{lowresBuffers[0][i]->GetAsBackend()};
		blitter.Blit(shaderBlurH.get(), vaoBlurH.get(), textures, nullptr, 0, lowresBuffers[1][i]->GetAsBackend());
	}

	// Vertical gaussian blur pass
	for (int i = 0; i < BlurIterations; i++)
	{
		const std::vector<Effekseer::Backend::TextureRef> textures{lowresBuffers[1][i]->GetAsBackend()};
		blitter.Blit(shaderBlurV.get(), vaoBlurV.get(), textures, nullptr, 0, lowresBuffers[0][i]->GetAsBackend());
	}

	// Blending pass
	state.AlphaBlend = AlphaBlendType::Add;
	renderer_->GetRenderState()->Update(false);
	{
		const std::vector<Effekseer::Backend::TextureRef> textures{lowresBuffers[0][0]->GetAsBackend(),
																   lowresBuffers[0][1]->GetAsBackend(),
																   lowresBuffers[0][2]->GetAsBackend(),
																   lowresBuffers[0][3]->GetAsBackend()};
		blitter.Blit(shaderBlend.get(), vaoBlend.get(), textures, nullptr, 0, dest, false);
	}

	GLExt::glActiveTexture(GL_TEXTURE0);
	renderer_->GetRenderState()->Update(true);
	renderer_->GetRenderState()->Pop();
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
		auto bufferSize = Effekseer::Tool::Vector2DI(width, height);
		extractBuffer.reset(RenderTexture::Create(graphics));
		extractBuffer->Initialize(bufferSize, Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT);
	}

	// Create low-resolution buffers
	for (int i = 0; i < BlurBuffers; i++)
	{
		auto bufferSize = Effekseer::Tool::Vector2DI(width, height);
		for (int j = 0; j < BlurIterations; j++)
		{
			bufferSize.X = std::max(1, (bufferSize.X + 1) / 2);
			bufferSize.Y = std::max(1, (bufferSize.Y + 1) / 2);
			lowresBuffers[i][j].reset(RenderTexture::Create(graphics));
			lowresBuffers[i][j]->Initialize(bufferSize, Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT);
		}
	}
}

void BloomEffectGL::ReleaseBuffers()
{
	renderTextureWidth = 0;
	renderTextureHeight = 0;

	extractBuffer.reset();
	for (int i = 0; i < BlurBuffers; i++)
	{
		for (int j = 0; j < BlurIterations; j++)
		{
			lowresBuffers[i][j].reset();
		}
	}
}

TonemapEffectGL::TonemapEffectGL(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: TonemapEffect(graphics)
	, blitter(graphics, renderer)
	, renderer_(EffekseerRendererGL::RendererImplementedRef::FromPinned(renderer.Get()))
{
	using namespace EffekseerRendererGL;

	auto vl = renderer->GetGraphicsDevice()->CreateVertexLayout(vlElem, 2).DownCast<EffekseerRendererGL::Backend::VertexLayout>();
	auto uniformLayoutSingleImage = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"u_Texture0"}, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>{});

	EffekseerRendererGL::ShaderCodeView basicVS(g_basic_vs_src);

	// Copy shader
	EffekseerRendererGL::ShaderCodeView copyPS(g_copy_fs_src);

	shaderCopy.reset(Shader::CreateWithHeader(renderer_->GetInternalGraphicsDevice(), basicVS, copyPS, uniformLayoutSingleImage, "Tonemap copy"));
	shaderCopy->SetVertexLayout(vl);

	// Reinhard shader
	EffekseerRendererGL::ShaderCodeView tonemapPS(g_tonemap_reinhard_fs_src);

	using namespace Effekseer::Backend;
	Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> uniformLayoutElm;
	uniformLayoutElm.emplace_back(UniformLayoutElement{ShaderStageType::Pixel, "u_Exposure", UniformBufferLayoutElementType::Vector4, 1, 0});
	auto uniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"u_Texture0"}, uniformLayoutElm);

	shaderReinhard.reset(EffekseerRendererGL::Shader::CreateWithHeader(renderer_->GetInternalGraphicsDevice(), basicVS, tonemapPS, uniformLayout, "Tonemap Reinhard"));
	shaderReinhard->SetVertexLayout(vl);
	shaderReinhard->SetPixelConstantBufferSize(sizeof(float) * 4);

	// Setup VAOs
	vaoCopy = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderCopy.get());
	vaoReinhard = blitter.CreateVAO(renderer->GetGraphicsDevice(), shaderReinhard.get());
}

TonemapEffectGL::~TonemapEffectGL()
{
}

void TonemapEffectGL::Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest)
{
	using namespace Effekseer;
	using namespace EffekseerRendererGL;

	auto& state = renderer_->GetRenderState()->Push();
	state.AlphaBlend = AlphaBlendType::Opacity;
	state.DepthWrite = false;
	state.DepthTest = false;
	state.CullingType = CullingType::Double;
	renderer_->GetRenderState()->Update(false);
	renderer_->SetRenderMode(RenderMode::Normal);

	const std::vector<Effekseer::Backend::TextureRef> textures{src};

	if (algorithm == Algorithm::Off)
	{
		blitter.Blit(shaderCopy.get(), vaoCopy.get(), textures, nullptr, 0, dest);
	}
	else if (algorithm == Algorithm::Reinhard)
	{
		const float constantData[4] = {exposure, 16.0f * 16.0f};
		blitter.Blit(shaderReinhard.get(), vaoReinhard.get(), textures, constantData, sizeof(constantData), dest);
	}

	GLExt::glActiveTexture(GL_TEXTURE0);
	renderer_->GetRenderState()->Update(true);
	renderer_->GetRenderState()->Pop();
	GLCheckError();
}

LinearToSRGBEffectGL::LinearToSRGBEffectGL(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: LinearToSRGBEffect(graphics)
	, blitter(graphics, renderer)
	, renderer_(EffekseerRendererGL::RendererImplementedRef::FromPinned(renderer.Get()))
{
	using namespace EffekseerRendererGL;

	auto vl = renderer->GetGraphicsDevice()->CreateVertexLayout(vlElem, 2).DownCast<EffekseerRendererGL::Backend::VertexLayout>();
	auto uniformLayoutSingleImage = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(Effekseer::CustomVector<Effekseer::CustomString<char>>{"u_Texture0"}, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>{});

	EffekseerRendererGL::ShaderCodeView basicVS(g_basic_vs_src);
	EffekseerRendererGL::ShaderCodeView linierToSrgbPS(g_linear_to_srgb_fs_src);

	// Copy shader
	shader_.reset(Shader::CreateWithHeader(renderer_->GetInternalGraphicsDevice(), basicVS, linierToSrgbPS, uniformLayoutSingleImage, "LinearToSRGB"));
	shader_->SetVertexLayout(vl);

	// Setup VAOs
	vao_ = blitter.CreateVAO(renderer->GetGraphicsDevice(), shader_.get());
}

LinearToSRGBEffectGL::~LinearToSRGBEffectGL()
{
}

void LinearToSRGBEffectGL::Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest)
{
	using namespace Effekseer;
	using namespace EffekseerRendererGL;

	auto& state = renderer_->GetRenderState()->Push();
	state.AlphaBlend = AlphaBlendType::Opacity;
	state.DepthWrite = false;
	state.DepthTest = false;
	state.CullingType = CullingType::Double;
	renderer_->GetRenderState()->Update(false);
	renderer_->SetRenderMode(RenderMode::Normal);

	const std::vector<Effekseer::Backend::TextureRef> textures{src};

	blitter.Blit(shader_.get(), vao_.get(), textures, nullptr, 0, dest);

	GLExt::glActiveTexture(GL_TEXTURE0);
	renderer_->GetRenderState()->Update(true);
	renderer_->GetRenderState()->Pop();
	GLCheckError();
}
} // namespace efk

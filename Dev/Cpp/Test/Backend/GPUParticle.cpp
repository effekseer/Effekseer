#ifdef _WIN32
#include <RenderingEnvironment/RenderingEnvironmentDX11.h>
#include <EffekseerRendererDX11.h>
#endif
#include <RenderingEnvironment/RenderingEnvironmentGL.h>

#include "../TestHelper.h"

#include "Effekseer.h"
#include <Effekseer/Utils/Effekseer.CustomAllocator.h>
#include <EffekseerRendererGL.h>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#define _USE_MATH_DEFINES
#include <Effekseer/Effekseer.Matrix44.h>
#include <math.h>

enum class GraphicsDeviceType
{
	OpenGL,
	DirectX11,
};

struct DataVertex
{
	std::array<float, 3> Particle;
	std::array<float, 3> Position;
	std::array<float, 3> Direction;
};

std::string ReadFileAll(std::string path)
{
	std::ifstream ifs(path);
	std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	return str;
}

std::array<uint8_t, 3> hsv2rgb(std::array<float, 3> hsv)
{
	float h = hsv[0] / 60;
	float s = hsv[1];
	float v = hsv[2];
	std::array<uint8_t, 3> rgb;

	if (s == 0)
	{
		return std::array<uint8_t, 3>{(uint8_t)(v * 255), (uint8_t)(v * 255), (uint8_t)(v * 255)};
	}

	int i = h;
	float f = h - i;

	uint8_t v0 = v * 255;
	uint8_t v1 = v * (1 - s) * 255;
	uint8_t v2 = v * (1 - s * f) * 255;
	uint8_t v3 = v * (1 - s * (1 - f)) * 255;

	switch (i)
	{
	case 0:;
	case 6:
		rgb = {v0, v3, v1};
		break;
	case 1:
		rgb = {v2, v0, v1};
		break;
	case 2:
		rgb = {v1, v0, v3};
		break;
	case 3:
		rgb = {v1, v2, v0};
		break;
	case 4:
		rgb = {v3, v1, v0};
		break;
	case 5:
		rgb = {v0, v1, v2};
		break;
	}
	return rgb;
}

int clz32(int num)
{
	return 32 - floor(log2(num)) - 1;
}

std::array<std::array<float, 4>, 4> matTo2DArray(Effekseer::Matrix44 matrix)
{
	std::array<std::array<float, 4>, 4> array2d;
	for (int i = 0; i < array2d.size(); i++)
	{
		for (int j = 0; j < array2d.size(); j++)
		{
			array2d[i][j] = matrix.Values[i][j];
		}
	}
	return array2d;
}

class GpuParticleBuffer
{
public:
	// textures[0]: Position
	// textures[1]: Velocity
	Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, 4> textures;
	Effekseer::Backend::RenderPassRef renderPass;

	GpuParticleBuffer(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int texWidth, int texHeight)
	{
		Effekseer::Backend::RenderTextureParameter texParam;
		texParam.Format = Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT;
		texParam.Size = {texWidth, texHeight};

		auto positionTexture = graphicsDevice->CreateRenderTexture(texParam);
		auto velocityTexture = graphicsDevice->CreateRenderTexture(texParam);
		textures.resize(2);
		textures.at(0) = positionTexture;
		textures.at(1) = velocityTexture;

		Effekseer::Backend::DepthTextureParameter depthTexParam;
		depthTexParam.Size = texParam.Size;

		auto depthTex = graphicsDevice->CreateDepthTexture(depthTexParam);
		renderPass = graphicsDevice->CreateRenderPass(textures, depthTex);
	}
};

class GpuParticleContext
{
	std::random_device seed_gen;
	std::mt19937 mt = std::mt19937(seed_gen());
	std::uniform_real_distribution<> rand1 = std::uniform_real_distribution<>(0, 1);
	static const int TrailBufferSize = 32;
	static const int EmitBufferSize = 1024;
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice;
	std::vector<GpuParticleBuffer> buffers;
	Effekseer::Backend::TextureRef colorTableTexture;
	int pingpong = 0;
	int maxParticleCount;
	int particleIndex = 0;
	std::array<DataVertex, EmitBufferSize> emitData;
	int emitedCount = 0;
	int newParticleCount = 0;
	int windowWidth = 1280;
	int windowHeight = 720;
	int texWidth;
	int texHeight;

	Effekseer::Backend::ShaderRef updateShader;
	Effekseer::Backend::ShaderRef renderShader;
	Effekseer::Backend::ShaderRef emitShader;
	Effekseer::Backend::RenderPassRef windowRenderPass;
	Effekseer::Backend::VertexBufferRef updateVB;
	Effekseer::Backend::VertexBufferRef emitVB;
	Effekseer::Backend::VertexBufferRef renderVB;
	Effekseer::Backend::IndexBufferRef updateIB;
	Effekseer::Backend::IndexBufferRef emitIB;
	Effekseer::Backend::IndexBufferRef renderIB;
	Effekseer::Backend::VertexLayoutRef updateVL;
	Effekseer::Backend::VertexLayoutRef emitVL;
	Effekseer::Backend::VertexLayoutRef renderVL;

	struct UpdateUniformBufferPS
	{
		std::array<float, 4> DeltaTime;
	};

	struct EmitUniformBufferVS
	{
		std::array<float, 4> ID2TPos;
		std::array<float, 4> TPos2VPos;
	};

	struct RenderUniformBufferVS
	{
		std::array<float, 4> ID2TPos;
		std::array<std::array<float, 4>, 4> ViewMatrix;
		std::array<std::array<float, 4>, 4> ProjMatrix;
	};
	Effekseer::Matrix44 viewMatrix;
	Effekseer::Matrix44 projMatrix;

	Effekseer::Backend::UniformBufferRef updateUniformBufferPS;
	Effekseer::Backend::UniformBufferRef emitUniformBufferVS;
	Effekseer::Backend::UniformBufferRef renderUniformBufferVS;

	bool trailMode;
	int32_t trailOffset = 0;
	Effekseer::Backend::TextureRef trailHistoriesTexture;
	Effekseer::Backend::VertexBufferRef trailVertexBuffer;
	Effekseer::Backend::IndexBufferRef trailRenderIndexBuffer;
	Effekseer::Backend::ShaderRef trailRenderShader;
	Effekseer::Backend::UniformBufferRef trailRenderUniformBufferVS;

	struct TrailRenderUniformBufferVS
	{
		std::array<float, 4> ID2TPos;
		std::array<std::array<float, 4>, 4> ViewMatrix;
		std::array<std::array<float, 4>, 4> ProjMatrix;
		std::array<float, 4> Trail;
	};

	void initRenderVertex()
	{
		std::array<std::array<float, 2>, 4> vbData;
		vbData[0] = {-1, 1};
		vbData[1] = {-1, -1};
		vbData[2] = {1, 1};
		vbData[3] = {1, -1};
		renderVB = graphicsDevice->CreateVertexBuffer(sizeof(std::array<float, 2>) * 4, vbData.data(), false);

		std::array<int32_t, 6> ibData;
		ibData[0] = 0;
		ibData[1] = 1;
		ibData[2] = 2;
		ibData[3] = 2;
		ibData[4] = 1;
		ibData[5] = 3;
		renderIB = graphicsDevice->CreateIndexBuffer(6, ibData.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);

		std::vector<Effekseer::Backend::VertexLayoutElement> vertexLayoutElements;
		vertexLayoutElements.resize(1);
		vertexLayoutElements[0].Format = Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
		vertexLayoutElements[0].Name = "a_VertexPosition";
		vertexLayoutElements[0].SemanticIndex = 0;
		vertexLayoutElements[0].SemanticName = "POSITION";
		renderVL = graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size()));
	}

	void initUpdateVertex()
	{
		std::array<std::array<float, 4>, 4> vbData; // {.x, .y, .u, .v}
		vbData[0] = {-1, 1, 0, 0};
		vbData[1] = {-1, -1, 0, 1};
		vbData[2] = {1, 1, 1, 0};
		vbData[3] = {1, -1, 1, 1};
		updateVB = graphicsDevice->CreateVertexBuffer(sizeof(std::array<float, 4>) * 4, vbData.data(), false);

		std::array<int32_t, 6> ibData;
		ibData[0] = 0;
		ibData[1] = 1;
		ibData[2] = 2;
		ibData[3] = 2;
		ibData[4] = 1;
		ibData[5] = 3;
		updateIB = graphicsDevice->CreateIndexBuffer(6, ibData.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);

		std::vector<Effekseer::Backend::VertexLayoutElement> vertexLayoutElements;
		vertexLayoutElements.resize(2);
		vertexLayoutElements[0].Format = Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
		vertexLayoutElements[0].Name = "a_Position";
		vertexLayoutElements[0].SemanticIndex = 0;
		vertexLayoutElements[0].SemanticName = "POSITION";
		vertexLayoutElements[1].Format = Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
		vertexLayoutElements[1].Name = "a_TexUV";
		vertexLayoutElements[1].SemanticIndex = 0;
		vertexLayoutElements[1].SemanticName = "TEXCOORD";
		updateVL = graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size()));
	}

	void initEmitVertex()
	{
		emitVB = graphicsDevice->CreateVertexBuffer(sizeof(DataVertex) * emitData.size(), emitData.data(), false);
		std::array<int32_t, EmitBufferSize> ibData;
		for (int i = 0; i < ibData.size(); i++)
		{
			ibData[i] = i;
		}
		emitIB = graphicsDevice->CreateIndexBuffer(ibData.size(), ibData.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);

		std::vector<Effekseer::Backend::VertexLayoutElement> vertexLayoutElements;
		vertexLayoutElements.resize(3);
		vertexLayoutElements[0].Format = Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
		vertexLayoutElements[0].Name = "a_Particle";
		vertexLayoutElements[0].SemanticIndex = 0;
		vertexLayoutElements[0].SemanticName = "POSITION";
		vertexLayoutElements[1].Format = Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
		vertexLayoutElements[1].Name = "a_Position";
		vertexLayoutElements[1].SemanticIndex = 1;
		vertexLayoutElements[1].SemanticName = "POSITION";
		vertexLayoutElements[2].Format = Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
		vertexLayoutElements[2].Name = "a_Direction";
		vertexLayoutElements[2].SemanticIndex = 2;
		vertexLayoutElements[2].SemanticName = "POSITION";

		emitVL = graphicsDevice->CreateVertexLayout(vertexLayoutElements.data(), static_cast<int32_t>(vertexLayoutElements.size()));
	}

	void initTrailHistoriesTexture()
	{
		if (trailMode)
		{
			Effekseer::Backend::TextureParameter param;
			param.Usage = Effekseer::Backend::TextureUsageType::Array;
			param.Format = Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT;
			param.Dimension = 2;
			param.Size = {texWidth, texHeight, TrailBufferSize};
			param.MipLevelCount = 1;
			param.SampleCount = 1;
			trailHistoriesTexture = graphicsDevice->CreateTexture(param);
			assert(trailHistoriesTexture != nullptr);
		}
	}

	void initTrailRenderVertexBuffer()
	{
		std::vector<Effekseer::Vector2D> trailVertexData;
		trailVertexData.resize((TrailBufferSize + 1) * 2);
		for (int i = 0; i <= TrailBufferSize; i++)
		{
			// Right
			trailVertexData[i * 2 + 0].X = static_cast<float>(+i) / TrailBufferSize;
			trailVertexData[i * 2 + 0].Y = +0.5;
			// Left
			trailVertexData[i * 2 + 1].X = static_cast<float>(+i) / TrailBufferSize;
			trailVertexData[i * 2 + 1].Y = -0.5;
		}
		trailVertexBuffer = graphicsDevice->CreateVertexBuffer(sizeof(Effekseer::Vector2D) * trailVertexData.size(), trailVertexData.data(), false);
		/*
					[0]   [1]   [2]         [n-1] [n]   n=TrailBufferSize+1

		+0.5   ->   +-----+-----+--...  ...--+-----+
					|     |     |            |     |
		Origin ->	*     *     *            *     *
					|     |     |            |     |
		-0.5   ->   +-----+-----+--...  ...--+-----+

		*/

		std::vector<int32_t> ibData;
		int quadCount = (TrailBufferSize + 1);
		ibData.resize(quadCount * 6);
		int32_t ofs = 0;
		for (int i = 0; i < quadCount; i++)
		{
			ibData[i * 6 + 0] = ofs + 0;
			ibData[i * 6 + 1] = ofs + 1;
			ibData[i * 6 + 2] = ofs + 2;
			ibData[i * 6 + 3] = ofs + 2;
			ibData[i * 6 + 4] = ofs + 1;
			ibData[i * 6 + 5] = ofs + 3;
			ofs += 2;
		}
		trailRenderIndexBuffer = graphicsDevice->CreateIndexBuffer(ibData.size(), ibData.data(), Effekseer::Backend::IndexBufferStrideType::Stride4);
	}

	void updateEmit(int targetIndex)
	{
		if (newParticleCount <= 0)
		{
			return;
		}

		initEmitVertex(); //Update Buffer‚Å‚â‚é‚×‚«

		Effekseer::Backend::PipelineStateParameter pipParam;

		// OpenGL doesn't require it
		pipParam.FrameBufferPtr = nullptr;
		pipParam.VertexLayoutPtr = emitVL;
		pipParam.ShaderPtr = emitShader;
		pipParam.IsDepthTestEnabled = false;
		pipParam.IsBlendEnabled = false;
		pipParam.Topology = Effekseer::Backend::TopologyType::Point;

		auto pip = graphicsDevice->CreatePipelineState(pipParam);

		graphicsDevice->SetViewport(0, 0, texWidth, texHeight);
		graphicsDevice->BeginRenderPass(buffers[targetIndex].renderPass, false, false, Effekseer::Color(0, 0, 0, 255));
		Effekseer::Backend::DrawParameter drawParam;

		drawParam.TextureCount = 0;
		drawParam.VertexBufferPtr = emitVB;
		drawParam.IndexBufferPtr = emitIB;
		drawParam.PipelineStatePtr = pip;
		drawParam.PrimitiveCount = newParticleCount;
		drawParam.InstanceCount = 1;
		drawParam.VertexUniformBufferPtr = emitUniformBufferVS;

		graphicsDevice->Draw(drawParam);
		graphicsDevice->EndRenderPass();

		emitedCount += newParticleCount;
		emitedCount %= maxParticleCount;
		newParticleCount = 0;
	}

	Effekseer::Backend::TextureRef createColorTableTexture()
	{
		const int width = 128;
		Effekseer::CustomVector<uint8_t> pixels(width * 4);
		for (int x = 0; x < width; x++)
		{
			const auto rgb = hsv2rgb(std::array<float, 3>{x * 360.0f / width, 0.8, 1.0});
			pixels[x * 4 + 0] = rgb[0];
			pixels[x * 4 + 1] = rgb[1];
			pixels[x * 4 + 2] = rgb[2];
			pixels[x * 4 + 3] = 255;
		}

		Effekseer::Backend::TextureParameter texParam;
		texParam.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		texParam.Size = {width, 1, 0};
		texParam.Dimension = 2;

		return graphicsDevice->CreateTexture(texParam, pixels);
	}

	void initUniformLayouts()
	{
		std::string DirectoryPath = GetDirectoryPath(__FILE__);
		Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> updateUniformLayoutElements;
		updateUniformLayoutElements.resize(1);
		updateUniformLayoutElements[0].Name = "DeltaTime";
		updateUniformLayoutElements[0].Offset = 0;
		updateUniformLayoutElements[0].Stage = Effekseer::Backend::ShaderStageType::Pixel;
		updateUniformLayoutElements[0].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;
		auto updateUniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(
			Effekseer::CustomVector<Effekseer::CustomString<char>>{"i_ParticleData0", "i_ParticleData1"},
			updateUniformLayoutElements);
		if (deviceType == GraphicsDeviceType::OpenGL)
		{
			updateShader = graphicsDevice->CreateShaderFromCodes(
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/perticle-update.vert.glsl").c_str()},
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/perticle-update.frag.glsl").c_str()},
				updateUniformLayout);
		}
		else
		{
			updateShader = graphicsDevice->CreateShaderFromCodes(
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/HLSL/perticle-update.vert.hlsl").c_str()},
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/HLSL/perticle-update.frag.hlsl").c_str()},
				updateUniformLayout);
		}
		assert(updateShader != nullptr);

		Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> emitUniformLayoutElements;
		emitUniformLayoutElements.resize(2);
		emitUniformLayoutElements[0].Name = "ID2TPos";
		emitUniformLayoutElements[0].Offset = 0;
		emitUniformLayoutElements[0].Stage = Effekseer::Backend::ShaderStageType::Vertex;
		emitUniformLayoutElements[0].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;
		emitUniformLayoutElements[1].Name = "TPos2VPos";
		emitUniformLayoutElements[1].Offset = sizeof(float) * 4;
		emitUniformLayoutElements[1].Stage = Effekseer::Backend::ShaderStageType::Vertex;
		emitUniformLayoutElements[1].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;
		auto emitUniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(
			Effekseer::CustomVector<Effekseer::CustomString<char>>{},
			emitUniformLayoutElements);
		if (deviceType == GraphicsDeviceType::OpenGL)
		{
			emitShader = graphicsDevice->CreateShaderFromCodes(
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/perticle-emit.vert.glsl").c_str()},
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/perticle-emit.frag.glsl").c_str()},
				emitUniformLayout);
		}
		else
		{
			emitShader = graphicsDevice->CreateShaderFromCodes(
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/HLSL/perticle-emit.vert.hlsl").c_str()},
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/HLSL/perticle-emit.frag.hlsl").c_str()},
				emitUniformLayout);
		}
		assert(emitShader != nullptr);

		Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> renderUniformLayoutElements;
		renderUniformLayoutElements.resize(3);
		renderUniformLayoutElements[0].Name = "ID2TPos";
		renderUniformLayoutElements[0].Offset = 0;
		renderUniformLayoutElements[0].Stage = Effekseer::Backend::ShaderStageType::Vertex;
		renderUniformLayoutElements[0].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;
		renderUniformLayoutElements[1].Name = "ViewMatrix";
		renderUniformLayoutElements[1].Offset = sizeof(float) * 4;
		renderUniformLayoutElements[1].Stage = Effekseer::Backend::ShaderStageType::Vertex;
		renderUniformLayoutElements[1].Type = Effekseer::Backend::UniformBufferLayoutElementType::Matrix44;
		renderUniformLayoutElements[2].Name = "ProjMatrix";
		renderUniformLayoutElements[2].Offset = sizeof(float) * 20;
		renderUniformLayoutElements[2].Stage = Effekseer::Backend::ShaderStageType::Vertex;
		renderUniformLayoutElements[2].Type = Effekseer::Backend::UniformBufferLayoutElementType::Matrix44;
		auto renderUniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(
			Effekseer::CustomVector<Effekseer::CustomString<char>>{"ParticleData0", "ParticleData1", "ColorTable"},
			renderUniformLayoutElements);
		if (deviceType == GraphicsDeviceType::OpenGL)
		{
			renderShader = graphicsDevice->CreateShaderFromCodes(
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/perticle-render.vert.glsl").c_str()},
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/perticle-render.frag.glsl").c_str()},
				renderUniformLayout);
		}
		else
		{
			renderShader = graphicsDevice->CreateShaderFromCodes(
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/HLSL/perticle-render.vert.hlsl").c_str()},
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/HLSL/perticle-render.frag.hlsl").c_str()},
				renderUniformLayout);
		}

		assert(renderShader != nullptr);
		colorTableTexture = createColorTableTexture();
		assert(colorTableTexture != nullptr);
	}

	void initTraitUniformLayoutAndShaders()
	{
		std::string DirectoryPath = GetDirectoryPath(__FILE__);
		Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement> renderUniformLayoutElements;
		renderUniformLayoutElements.resize(4);
		renderUniformLayoutElements[0].Name = "ID2TPos";
		renderUniformLayoutElements[0].Offset = 0;
		renderUniformLayoutElements[0].Stage = Effekseer::Backend::ShaderStageType::Vertex;
		renderUniformLayoutElements[0].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;
		renderUniformLayoutElements[1].Name = "ViewMatrix";
		renderUniformLayoutElements[1].Offset = sizeof(float) * 4;
		renderUniformLayoutElements[1].Stage = Effekseer::Backend::ShaderStageType::Vertex;
		renderUniformLayoutElements[1].Type = Effekseer::Backend::UniformBufferLayoutElementType::Matrix44;
		renderUniformLayoutElements[2].Name = "ProjMatrix";
		renderUniformLayoutElements[2].Offset = sizeof(float) * 20;
		renderUniformLayoutElements[2].Stage = Effekseer::Backend::ShaderStageType::Vertex;
		renderUniformLayoutElements[2].Type = Effekseer::Backend::UniformBufferLayoutElementType::Matrix44;
		renderUniformLayoutElements[3].Name = "Trail";
		renderUniformLayoutElements[3].Offset = sizeof(float) * 36;
		renderUniformLayoutElements[3].Stage = Effekseer::Backend::ShaderStageType::Vertex;
		renderUniformLayoutElements[3].Type = Effekseer::Backend::UniformBufferLayoutElementType::Vector4;
		auto renderUniformLayout = Effekseer::MakeRefPtr<Effekseer::Backend::UniformLayout>(
			Effekseer::CustomVector<Effekseer::CustomString<char>>{"ParticleData0", "ParticleData1", "ColorTable", "Histories"},
			renderUniformLayoutElements);
		if (deviceType == GraphicsDeviceType::OpenGL)
		{
			trailRenderShader = graphicsDevice->CreateShaderFromCodes(
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/trail-render.vert.glsl").c_str()},
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/trail-render.frag.glsl").c_str()},
				renderUniformLayout);
		}
		else
		{
			trailRenderShader = graphicsDevice->CreateShaderFromCodes(
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/HLSL/trail-render.vert.hlsl").c_str()},
				{ReadFileAll(DirectoryPath + "GpuParticleShaders/HLSL/trail-render.frag.hlsl").c_str()},
				renderUniformLayout);
		}
		assert(trailRenderShader != nullptr);

		TrailRenderUniformBufferVS trailRenderUniformBufferVSInitData = {
			{static_cast<float>(texWidth - 1), static_cast<float>(31 - clz32(texWidth)), 0, 0}, matTo2DArray(viewMatrix), matTo2DArray(projMatrix), {static_cast<float>(trailOffset), static_cast<float>(TrailBufferSize)}};
		trailRenderUniformBufferVS = graphicsDevice->CreateUniformBuffer(sizeof(TrailRenderUniformBufferVS), &trailRenderUniformBufferVSInitData);
	}

public:
	std::shared_ptr<RenderingEnvironment> window;
	GraphicsDeviceType deviceType;

	GpuParticleContext(GraphicsDeviceType deviceType, int maxParticleCount, int windowWidth, int windowHeight, bool isTrailMode)
	{
		this->trailMode = isTrailMode;
		this->deviceType = deviceType;
		this->windowWidth = windowWidth;
		this->windowHeight = windowHeight;

		texWidth = TrailBufferSize;
		texHeight = ((maxParticleCount + texWidth - 1) / texWidth) | 0;
		this->maxParticleCount = texWidth * texHeight;

		if (deviceType == GraphicsDeviceType::OpenGL)
		{
			window = std::make_shared<RenderingEnvironmentGL>(std::array<int, 2>({windowWidth, windowHeight}), "Backend.GpuParticle");
		}
		else if (deviceType == GraphicsDeviceType::DirectX11)
		{
#ifdef _WIN32
			window = std::make_shared<RenderingEnvironmentDX11>(std::array<int, 2>({windowWidth, windowHeight}), "Backend.GpuParticle");
#endif
		}
		graphicsDevice = window->GetGraphicsDevice();
		buffers.push_back(GpuParticleBuffer(graphicsDevice, texWidth, texHeight));
		buffers.push_back(GpuParticleBuffer(graphicsDevice, texWidth, texHeight));

		initUniformLayouts();

		UpdateUniformBufferPS updateUniformBufferPSInitData = {{1.0, 0, 0, 0}};
		updateUniformBufferPS = graphicsDevice->CreateUniformBuffer(sizeof(UpdateUniformBufferPS), &updateUniformBufferPSInitData);
		EmitUniformBufferVS emitUniformBufferVSInitData = {{static_cast<float>(texWidth - 1), static_cast<float>(31 - clz32(texWidth)), 0, 0},
														   {2.0f / texWidth, 2.0f / texHeight, 1.0f / texWidth - 1.0f, 1.0f / texHeight - 1.0f}};
		emitUniformBufferVS = graphicsDevice->CreateUniformBuffer(sizeof(EmitUniformBufferVS), &emitUniformBufferVSInitData);
		projMatrix.Indentity();
		viewMatrix.Indentity();

		RenderUniformBufferVS renderUniformBufferVSInitData = {{static_cast<float>(texWidth - 1), static_cast<float>(31 - clz32(texWidth)), 0, 0}, matTo2DArray(viewMatrix), matTo2DArray(projMatrix)};
		renderUniformBufferVS = graphicsDevice->CreateUniformBuffer(sizeof(RenderUniformBufferVS), &renderUniformBufferVSInitData);

		windowRenderPass = window->GetScreenRenderPass();

		initUpdateVertex();
		initRenderVertex();
		initEmitVertex();

		initTrailHistoriesTexture();
		initTrailRenderVertexBuffer();
		initTraitUniformLayoutAndShaders();
	}

	void Emit(int lifeTime, std::array<float, 3> position, std::array<float, 3> direction)
	{
		const int index = this->newParticleCount;
		this->emitData[index].Particle[0] = (this->emitedCount + this->newParticleCount) % this->maxParticleCount;
		this->emitData[index].Particle[1] = lifeTime;
		this->emitData[index].Particle[2] = rand1(mt);
		this->emitData[index].Position = position;
		this->emitData[index].Direction = direction;
		this->newParticleCount++;
		this->particleIndex++;
	}

	void Update()
	{
		const int sourceIndex = pingpong;
		const int targetIndex = (pingpong + 1) % 2;

		if (trailMode)
		{
			if (--trailOffset < 0)
			{
				trailOffset = TrailBufferSize - 1;
			}

			//gl.viewport(0, 0, this.texWidth, this.texHeight);
			//gl.useProgram(this.trailUpdateShader);
			//gl.bindFramebuffer(gl.FRAMEBUFFER, this.trailFrameBuffer);
			//this.buffers[sourceIndex].setUpdateSource();
			//gl.uniform1i(gl.getUniformLocation(this.trailUpdateShader, "i_ParticleData0"), 0);
			//gl.uniform1i(gl.getUniformLocation(this.trailUpdateShader, "i_ParticleData1"), 1);
			//gl.framebufferTextureLayer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, this.trailBufferTexture, 0, this.trailOffset);
			//gl.bindBuffer(gl.ARRAY_BUFFER, this.quadBuffer);
			//gl.enableVertexAttribArray(0);
			//gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 0, 0);
			//gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
			//gl.disableVertexAttribArray(0);
			//gl.bindFramebuffer(gl.FRAMEBUFFER, null);

			// Position texture to
			graphicsDevice->CopyTexture(trailHistoriesTexture, buffers[sourceIndex].textures.at(0), {0, 0, 0}, {0, 0, 0}, {texWidth, texHeight, 1}, trailOffset, 0);

			//gl.bindFramebuffer(gl.FRAMEBUFFER, );
			//gl.bindTexture(gl.TEXTURE_2D_ARRAY, this.trailBufferTexture, );
			//gl.copyTexSubImage3D(gl.TEXTURE_2D_ARRAY, 0, 0, 0, this.trailOffset, 0, 0, this.texWidth, this.texHeight);
			//gl.bindTexture(gl.TEXTURE_2D_ARRAY, null);
			//gl.bindFramebuffer(gl.FRAMEBUFFER, null);
		}

		Effekseer::Backend::PipelineStateParameter pipParam;

		// OpenGL doesn't require it
		pipParam.FrameBufferPtr = nullptr;
		pipParam.VertexLayoutPtr = updateVL;
		pipParam.ShaderPtr = updateShader;
		pipParam.IsDepthTestEnabled = false;
		pipParam.IsBlendEnabled = false;

		auto pip = graphicsDevice->CreatePipelineState(pipParam);

		graphicsDevice->SetViewport(0, 0, texWidth, texHeight);
		graphicsDevice->BeginRenderPass(buffers[targetIndex].renderPass, true, true, Effekseer::Color(0, 0, 0, 255));

		if (trailMode)
		{
			TrailRenderUniformBufferVS trailRenderUniformBufferVSInitData = {
				{static_cast<float>(texWidth - 1), static_cast<float>(31 - clz32(texWidth)), 0, 0}, matTo2DArray(viewMatrix), matTo2DArray(projMatrix), {static_cast<float>(trailOffset), static_cast<float>(TrailBufferSize), 0.0f, 0.0f}};
			graphicsDevice->UpdateUniformBuffer(trailRenderUniformBufferVS, sizeof(TrailRenderUniformBufferVS), 0, &trailRenderUniformBufferVSInitData);
		}
		else
		{
			RenderUniformBufferVS renderUniformBufferVSData = {
				{static_cast<float>(texWidth - 1), static_cast<float>(31 - clz32(texWidth)), 0, 0}, matTo2DArray(viewMatrix), matTo2DArray(projMatrix)};
			graphicsDevice->UpdateUniformBuffer(renderUniformBufferVS, sizeof(RenderUniformBufferVS), 0, &renderUniformBufferVSData);
		}

		Effekseer::Backend::DrawParameter drawParam;

		drawParam.TextureCount = buffers[sourceIndex].textures.size();

		for (int i = 0; i < drawParam.TextureCount; i++)
		{
			drawParam.TexturePtrs[i] = buffers[sourceIndex].textures.at(i);
			drawParam.TextureSamplingTypes[i] = Effekseer::Backend::TextureSamplingType::Nearest;
			drawParam.TextureWrapTypes[i] = Effekseer::Backend::TextureWrapType::Clamp;
		}
		drawParam.PipelineStatePtr = pip;

		drawParam.VertexBufferPtr = updateVB;
		drawParam.IndexBufferPtr = updateIB;
		drawParam.PipelineStatePtr = pip;
		drawParam.PrimitiveCount = 2;
		drawParam.InstanceCount = 1;
		drawParam.PixelUniformBufferPtr = updateUniformBufferPS;

		graphicsDevice->Draw(drawParam);
		graphicsDevice->EndRenderPass();

		updateEmit(targetIndex);

		pingpong = (pingpong + 1) % 2;

		projMatrix.PerspectiveFovRH(30 * M_PI / 180, (float)windowWidth / windowHeight, 0.1, 100);
		viewMatrix.LookAtRH(Effekseer::Vector3D(0, 2, 2), Effekseer::Vector3D(0, 0, 0), Effekseer::Vector3D(0, 1, 0));
	}

	void Render()
	{
		Effekseer::Backend::PipelineStateRef pip;
		if (trailMode)
		{
			Effekseer::Backend::PipelineStateParameter pipParam;
			pipParam.FrameBufferPtr = nullptr;
			pipParam.VertexLayoutPtr = renderVL;
			pipParam.ShaderPtr = trailRenderShader;
			pipParam.IsDepthTestEnabled = false;
			pipParam.IsBlendEnabled = true;
			pipParam.BlendSrcFunc = Effekseer::Backend::BlendFuncType::SrcAlpha;
			pipParam.BlendDstFunc = Effekseer::Backend::BlendFuncType::One;
			pipParam.BlendSrcFuncAlpha = Effekseer::Backend::BlendFuncType::SrcAlpha;
			pipParam.BlendDstFuncAlpha = Effekseer::Backend::BlendFuncType::One;
			pip = graphicsDevice->CreatePipelineState(pipParam);
		}
		else
		{
			Effekseer::Backend::PipelineStateParameter pipParam;
			// OpenGL doesn't require it
			pipParam.FrameBufferPtr = nullptr;
			pipParam.VertexLayoutPtr = renderVL;
			pipParam.ShaderPtr = renderShader;
			pipParam.IsDepthTestEnabled = false;
			pipParam.IsBlendEnabled = true;
			pipParam.BlendSrcFunc = Effekseer::Backend::BlendFuncType::SrcAlpha;
			pipParam.BlendDstFunc = Effekseer::Backend::BlendFuncType::One;
			pipParam.BlendSrcFuncAlpha = Effekseer::Backend::BlendFuncType::SrcAlpha;
			pipParam.BlendDstFuncAlpha = Effekseer::Backend::BlendFuncType::One;
			pip = graphicsDevice->CreatePipelineState(pipParam);
		}

		graphicsDevice->SetViewport(0, 0, windowWidth, windowHeight);
		graphicsDevice->BeginRenderPass(windowRenderPass, true, true, Effekseer::Color(0, 0, 0, 255));
		Effekseer::Backend::DrawParameter drawParam;
		drawParam.TextureCount = buffers[pingpong].textures.size() + 1;
		for (int i = 0; i < buffers[pingpong].textures.size(); i++)
		{
			drawParam.TexturePtrs[i] = buffers[pingpong].textures.at(i);
			drawParam.TextureSamplingTypes[i] = Effekseer::Backend::TextureSamplingType::Nearest;
			drawParam.TextureWrapTypes[i] = Effekseer::Backend::TextureWrapType::Clamp;
		}
		int colorTableTextureIndex = buffers[pingpong].textures.size();
		drawParam.TexturePtrs[colorTableTextureIndex] = colorTableTexture;
		drawParam.TextureSamplingTypes[colorTableTextureIndex] = Effekseer::Backend::TextureSamplingType::Linear;
		drawParam.TextureWrapTypes[colorTableTextureIndex] = Effekseer::Backend::TextureWrapType::Clamp;

		if (trailMode)
		{
			drawParam.TextureCount++;
			int historiesTextureIndex = colorTableTextureIndex + 1;
			drawParam.TexturePtrs[historiesTextureIndex] = trailHistoriesTexture;
			drawParam.TextureSamplingTypes[historiesTextureIndex] = Effekseer::Backend::TextureSamplingType::Linear;
			drawParam.TextureWrapTypes[historiesTextureIndex] = Effekseer::Backend::TextureWrapType::Clamp;

			drawParam.VertexBufferPtr = trailVertexBuffer;
			drawParam.IndexBufferPtr = trailRenderIndexBuffer;
			drawParam.PipelineStatePtr = pip;
			drawParam.PrimitiveCount = TrailBufferSize * 2;
			drawParam.InstanceCount = maxParticleCount;
			drawParam.VertexUniformBufferPtr = trailRenderUniformBufferVS;

			graphicsDevice->Draw(drawParam);
		}
		else
		{
			drawParam.VertexBufferPtr = renderVB;
			drawParam.IndexBufferPtr = renderIB;
			drawParam.PipelineStatePtr = pip;
			drawParam.PrimitiveCount = 2;
			drawParam.InstanceCount = maxParticleCount;
			drawParam.VertexUniformBufferPtr = renderUniformBufferVS;

			graphicsDevice->Draw(drawParam);
		}

		graphicsDevice->EndRenderPass();
	}
};

void GpuParticle(GraphicsDeviceType deviceType, bool trailMode)
{
	int windowWidth = 1280;
	int windowHeight = 720;
	GpuParticleContext gpuParticleContext(deviceType, 1024 * 128, windowWidth, windowHeight, trailMode);
	int count = 0;
	int emitNum = 128;
	while (count < 1000 && gpuParticleContext.window->DoEvent())
	{
		for (int i = 0; i < emitNum; i++)
		{
			std::array<float, 3> position = {-0.5f - (i - emitNum / 2) * (i - emitNum / 2) / 10.0f / emitNum, -1 + i * 2.0f / emitNum, 0};
			std::array<float, 3> direction = {1, 1, 0};
			gpuParticleContext.Emit(500, position, direction);
		}

		gpuParticleContext.Update();

		gpuParticleContext.Render();

		gpuParticleContext.window->Present();

		count++;
	}
}

#if !defined(__FROM_CI__)

TestRegister Test_GpuParticle_GL("Backend.GpuParticle_GL", []() -> void { GpuParticle(GraphicsDeviceType::OpenGL, false); });
TestRegister Test_GpuParticleTrail_GL("Backend.GpuParticle_Trail_GL", []() -> void { GpuParticle(GraphicsDeviceType::OpenGL, true); });

#ifdef _WIN32

TestRegister Test_GpuParticle_DX11("Backend.GpuParticle_DX11", []() -> void { GpuParticle(GraphicsDeviceType::DirectX11, false); });
TestRegister Test_GpuParticleTrail_DX11("Backend.GpuParticle_Trail_DX11", []() -> void { GpuParticle(GraphicsDeviceType::DirectX11, true); });

#endif
#endif

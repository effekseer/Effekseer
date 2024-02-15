#include "EffekseerRenderer.GpuParticles.h"
#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.Renderer_Impl.h"
#include "../Effekseer/Effekseer/Noise/CurlNoise.h"

namespace EffekseerRenderer
{

namespace
{

inline constexpr uint32_t RoundUp(uint32_t x, uint32_t unit)
{
	return (x + unit - 1) / unit * unit;
}

inline uint32_t PackNormal(GpuParticles::float3 v)
{
	uint32_t x = std::clamp(uint32_t((v.x + 1.0f) * 0.5f * 1023.0f), 0u, 1023u);
	uint32_t y = std::clamp(uint32_t((v.y + 1.0f) * 0.5f * 1023.0f), 0u, 1023u);
	uint32_t z = std::clamp(uint32_t((v.z + 1.0f) * 0.5f * 1023.0f), 0u, 1023u);
	return x | (y << 10) | (z << 20);
}

inline uint32_t PackUV(GpuParticles::float2 v)
{
	uint32_t x = std::clamp(uint32_t(v.x * 65535.0f), 0u, 65535u);
	uint32_t y = std::clamp(uint32_t(v.y * 65535.0f), 0u, 65535u);
	return x | (y << 16);
}

inline uint32_t PackColor(GpuParticles::float4 v)
{
	uint32_t x = std::clamp(uint32_t(v.x * 255.0f), 0u, 255u);
	uint32_t y = std::clamp(uint32_t(v.y * 255.0f), 0u, 255u);
	uint32_t z = std::clamp(uint32_t(v.y * 255.0f), 0u, 255u);
	uint32_t w = std::clamp(uint32_t(v.y * 255.0f), 0u, 255u);
	return x | (y << 8) | (z << 16) | (w << 24);
}

inline GpuParticles::float2 operator+(GpuParticles::float2 a, GpuParticles::float2 b) {
	return GpuParticles::float2{ a.x + b.x, a.y + b.y };
}

inline GpuParticles::float2 operator*(GpuParticles::float2 a, float b) {
	return GpuParticles::float2{ a.x * b, a.y * b };
}

inline GpuParticles::float3 operator+(GpuParticles::float3 a, GpuParticles::float3 b) {
	return GpuParticles::float3{ a.x + b.x, a.y + b.y, a.z + b.z };
}

inline GpuParticles::float3 operator*(GpuParticles::float3 a, float b) {
	return GpuParticles::float3{ a.x * b, a.y * b, a.z * b };
}

inline GpuParticles::float4 operator+(GpuParticles::float4 a, GpuParticles::float4 b) {
	return GpuParticles::float4{ a.x + b.x, a.y + b.y, a.z + b.z , a.w + b.w };
}

inline GpuParticles::float4 operator*(GpuParticles::float4 a, float b) {
	return GpuParticles::float4{ a.x * b, a.y * b, a.z * b, a.w * b };
}

struct Random {
	std::mt19937 engine;
	std::uniform_real_distribution<float> dist;
	
	Random(uint32_t seed): engine(seed), dist(0.0f, 1.0f) {
	}

	float operator()() {
		return dist(engine);
	}
};

template <class T>
T RandomTriangle(Random& random, T a, T b, T c) {
	float u = random(), v = random();
	float t = std::min(u, v), s = std::max(u, v);
	float ma = t, mb = 1.0f - s, mc = s - t;
	return a * ma + b * mb + c * mc;
}

const Effekseer::Vector3D VEC_UP = {0.0f, 1.0f, 0.0f};
const Effekseer::Vector3D VEC_RIGHT = {1.0f, 0.0f, 0.0f};
const Effekseer::Vector3D VEC_FRONT = {0.0f, 0.0f, 1.0f};
const Effekseer::Color COLOR_WHITE = {255, 255, 255, 255};

Effekseer::ModelRef CreateSpriteModel()
{
	Effekseer::CustomVector<Effekseer::Model::Vertex> vertexes = {
		{ { -0.5f,  0.5f, 0.0f }, VEC_FRONT, VEC_UP, VEC_RIGHT, {0.0f, 0.0f}, COLOR_WHITE },
		{ { -0.5f, -0.5f, 0.0f }, VEC_FRONT, VEC_UP, VEC_RIGHT, {0.0f, 1.0f}, COLOR_WHITE },
		{ {  0.5f,  0.5f, 0.0f }, VEC_FRONT, VEC_UP, VEC_RIGHT, {1.0f, 0.0f}, COLOR_WHITE },
		{ {  0.5f, -0.5f, 0.0f }, VEC_FRONT, VEC_UP, VEC_RIGHT, {1.0f, 1.0f}, COLOR_WHITE },
	};
	Effekseer::CustomVector<Effekseer::Model::Face> faces = {
		{ {0, 2, 1} }, { {1, 2, 3} }
	};
	return Effekseer::MakeRefPtr<Effekseer::Model>(vertexes, faces);
}

Effekseer::ModelRef CreateTrailModel()
{
	const size_t TrailJoints = 256;
	Effekseer::CustomVector<Effekseer::Model::Vertex> vertexes;

	vertexes.resize(TrailJoints * 2);
	for (size_t i = 0; i < TrailJoints * 2; i++) {
		Effekseer::Model::Vertex v{};
		v.Position.X = (i % 2 == 0) ? -0.5f : 0.5f;
		v.Position.Y = 0.0f;
		v.Position.Z = 0.0f;
		v.Binormal = VEC_FRONT;
		v.Normal = VEC_UP;
		v.Tangent = VEC_RIGHT;
		v.UV.X = (i % 2 == 0) ? 0.0f : 1.0f;
		v.UV.Y = (float)(i / 2) / (float)(TrailJoints - 1);
		v.VColor = COLOR_WHITE;
		vertexes[i] = v;
	}

	Effekseer::CustomVector<Effekseer::Model::Face> faces;
	faces.resize((TrailJoints - 1) * 2);
	for (size_t i = 0; i < TrailJoints - 1; i++) {
		int32_t ofs = (int32_t)(2 * i);
		faces[i * 2 + 0] = Effekseer::Model::Face{ { ofs + 0, ofs + 2, ofs + 1 } };
		faces[i * 2 + 1] = Effekseer::Model::Face{ { ofs + 1, ofs + 2, ofs + 3 } };
	}
	return Effekseer::MakeRefPtr<Effekseer::Model>(vertexes, faces);
}

std::vector<GpuParticles::EmitPoint> GeneratePointCache(Effekseer::ModelRef model, uint32_t pointCount, uint32_t seed)
{
	std::vector<std::vector<float>> modelFaceAreas;
	float totalArea = 0.0f;

	int32_t frameCount = model->GetFrameCount();
	modelFaceAreas.resize((size_t)frameCount);

	for (int32_t frameIndex = 0; frameIndex < frameCount; frameIndex++)
	{
		auto vertexes = model->GetVertexes(frameIndex);
		int32_t vertexCount = model->GetVertexCount(frameIndex);
		auto faces = model->GetFaces(frameIndex);
		int32_t faceCount = model->GetFaceCount(frameIndex);

		auto& faceAreas = modelFaceAreas[frameIndex];
		faceAreas.resize((size_t)faceCount);

		for (int32_t faceIndex = 0; faceIndex < faceCount; faceIndex++)
		{
			auto& v0 = vertexes[faces[faceIndex].Indexes[0]];
			auto& v1 = vertexes[faces[faceIndex].Indexes[1]];
			auto& v2 = vertexes[faces[faceIndex].Indexes[2]];
			float r0 = Effekseer::Vector3D::Length(v0.Position - v1.Position);
			float r1 = Effekseer::Vector3D::Length(v1.Position - v2.Position);
			float r2 = Effekseer::Vector3D::Length(v2.Position - v0.Position);
			float s = (r0 + r1 + r2) / 2.0f;
			float area = sqrt(s * (s - r0) * (s - r1) * (s - r2));
			totalArea += area;
			faceAreas[faceIndex] = area;
		}
	}
	
	Random random(seed);
	std::vector<GpuParticles::EmitPoint> points;
	points.resize(pointCount);

	uint32_t pointIndex = 0;
	float summedArea = 0.0f;
	for (int32_t frameIndex = 0; frameIndex < frameCount; frameIndex++)
	{
		auto vertexes = model->GetVertexes(frameIndex);
		int32_t vertexCount = model->GetVertexCount(frameIndex);
		auto faces = model->GetFaces(frameIndex);
		int32_t faceCount = model->GetFaceCount(frameIndex);

		auto& faceAreas = modelFaceAreas[frameIndex];

		for (int32_t faceIndex = 0; faceIndex < faceCount; faceIndex++)
		{
			auto& v0 = vertexes[faces[faceIndex].Indexes[0]];
			auto& v1 = vertexes[faces[faceIndex].Indexes[1]];
			auto& v2 = vertexes[faces[faceIndex].Indexes[2]];

			summedArea += faceAreas[faceIndex];

			uint32_t genCount = (uint32_t)(summedArea / totalArea * pointCount) - pointIndex;
			for (uint32_t i = 0; i < genCount; i++)
			{
				GpuParticles::EmitPoint& point = points[(size_t)pointIndex + i];
				point.Position = RandomTriangle<GpuParticles::float3>(random, v0.Position, v1.Position, v2.Position);
				point.Normal = PackNormal(RandomTriangle<GpuParticles::float3>(random, v0.Normal, v1.Normal, v2.Normal));
				point.Binormal = PackNormal(RandomTriangle<GpuParticles::float3>(random, v0.Binormal, v1.Binormal, v2.Binormal));
				point.Tangent = PackNormal(RandomTriangle<GpuParticles::float3>(random, v0.Tangent, v1.Tangent, v2.Tangent));
				point.UV = PackUV(RandomTriangle<GpuParticles::float2>(random, v0.UV, v1.UV, v2.UV));
				point.VColor = PackColor(RandomTriangle<GpuParticles::float4>(random, v0.VColor.ToFloat4(), v1.VColor.ToFloat4(), v2.VColor.ToFloat4()));
			}
			pointIndex += genCount;
		}
	}

	return points;
}

}

void GpuParticles::BlockAllocator::Init(uint32_t bufferSize, uint32_t blockSize)
{
	bufferBlocks.reserve(bufferSize / blockSize);
	bufferBlocks.push_back({0, bufferSize});
}

GpuParticles::Block GpuParticles::BlockAllocator::Allocate(uint32_t size)
{
	Block result{};
	for (size_t i = 0; i < bufferBlocks.size(); i++)
	{
		Block& block = bufferBlocks[i];
		if (block.size >= size)
		{
			if (block.size == size)
			{
				result = block;
				bufferBlocks.erase(bufferBlocks.begin() + i);
				break;
			}
			else
			{
				result = Block{block.offset, size};
				block.offset += size;
				break;
			}
		}
	}
	return result;
}

void GpuParticles::BlockAllocator::Deallocate(Block releasingBlock)
{
	if (releasingBlock.size == 0)
	{
		return;
	}

	Block newBlock = releasingBlock;
	uint32_t newTail = newBlock.offset + newBlock.size;
	for (size_t i = 0; i < bufferBlocks.size(); i++)
	{
		Block& block = bufferBlocks[i];
		if (block.offset + block.size == newBlock.offset)
		{
			block.size += newBlock.size;

			if (i + 1 < bufferBlocks.size() && block.offset + block.size == bufferBlocks[i + 1].offset)
			{
				block.size += bufferBlocks[i + 1].size;
				bufferBlocks.erase(bufferBlocks.begin() + i + 1);
			}
			break;
		}
		else if (newBlock.offset + newBlock.size == block.offset)
		{
			block.offset -= newBlock.size;
			break;
		}
		else if (newBlock.offset < block.offset)
		{
			bufferBlocks.insert(bufferBlocks.begin() + i, newBlock);
			break;
		}
	}
}

GpuParticles::GpuParticles(Renderer* renderer)
	: m_rendererBase(renderer)
{
}

GpuParticles::~GpuParticles()
{
}

bool GpuParticles::InitSystem(const Settings& settings)
{
	auto graphics = m_rendererBase->GetGraphicsDevice();

	m_settings = settings;
	m_emitters.resize(settings.EmitterMaxCount);

	for (uint32_t index = 0; index < settings.EmitterMaxCount; index++)
	{
		m_emitterFreeList.push_back(index);
		m_emitters[index].buffer = graphics->CreateUniformBuffer(sizeof(EmitterData), nullptr);
	}
	m_newEmitterIDs.reserve(settings.EmitterMaxCount);

	m_particleAllocator.Init(settings.ParticleMaxCount, ParticleUnitSize);
	m_trailAllocator.Init(settings.TrailMaxCount, ParticleUnitSize);

	DrawConstants cdata{};
	m_ubufConstants = graphics->CreateUniformBuffer(sizeof(DrawConstants), &cdata);
	m_cbufParticles = graphics->CreateComputeBuffer((int32_t)settings.ParticleMaxCount, (int32_t)sizeof(Particle), nullptr, false);
	m_cbufTrails = graphics->CreateComputeBuffer((int32_t)settings.TrailMaxCount, (int32_t)sizeof(Trail), nullptr, false);

	m_vertexLayout = EffekseerRenderer::GetModelRendererVertexLayout(graphics);

	m_modelSprite = CreateSpriteModel();
	m_modelSprite->StoreBufferToGPU(graphics.Get());

	m_modelTrail = CreateTrailModel();
	m_modelTrail->StoreBufferToGPU(graphics.Get());

	{
		Effekseer::Backend::TextureParameter texParam{};
		texParam.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		texParam.Size = { 1, 1, 1 };
		texParam.Dimension = 3;
		m_dummyVectorTexture = graphics->CreateTexture(texParam, {0, 0, 0, 0});
	}
	{
		EmitPoint dummyData = {};
		m_dummyEmitPoints = graphics->CreateComputeBuffer(1, sizeof(dummyData), &dummyData, true);
	}

	m_dummyColorTexture = m_rendererBase->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
	m_dummyNormalTexture = m_rendererBase->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);

	return true;
}

void GpuParticles::SetShaders(const Shaders& shaders)
{
	m_shaders = shaders;

	auto graphics = m_rendererBase->GetGraphicsDevice();

	{
		Effekseer::Backend::PipelineStateParameter params{};
		params.ShaderPtr = m_shaders.csParticleClear;
		m_pipelineParticleClear = graphics->CreatePipelineState(params);
	}
	{
		Effekseer::Backend::PipelineStateParameter params{};
		params.ShaderPtr = m_shaders.csParticleSpawn;
		m_pipelineParticleSpawn = graphics->CreatePipelineState(params);
	}
	{
		Effekseer::Backend::PipelineStateParameter params{};
		params.ShaderPtr = m_shaders.csParticleUpdate;
		m_pipelineParticleUpdate = graphics->CreatePipelineState(params);
	}
}

void GpuParticles::ComputeFrame()
{
	auto renderer = m_rendererBase;
	auto graphics = renderer->GetGraphicsDevice();

	for (auto emitterID : m_newEmitterIDs)
	{
		// Initialize particle data region
		auto& emitter = m_emitters[emitterID];
		graphics->UpdateUniformBuffer(emitter.buffer, sizeof(EmitterData), 0, &emitter.data);

		ComputeCommand command;
		command.PipelineStatePtr = m_pipelineParticleClear;

		command.UniformBufferPtrs[0] = emitter.resource->paramBuffer;
		command.UniformBufferPtrs[1] = emitter.buffer;
		command.SetComputeBuffer(0, m_cbufParticles, false);

		command.GroupCount = { (int32_t)emitter.data.ParticleSize / 256, 1, 1 };
		command.ThreadCount = { 256, 1, 1 };
		graphics->Dispatch(command);
	}
	m_newEmitterIDs.clear();

	// Spawn particles
	for (EmitterID emitterID = 0; emitterID < (EmitterID)m_emitters.size(); emitterID++)
	{
		auto& emitter = m_emitters[emitterID];
		if (emitter.IsAlive())
		{
			auto& paramSet = emitter.resource->paramSet;

			emitter.data.TimeCount += emitter.data.DeltaTime;

			emitter.data.TotalEmitCount += emitter.data.NextEmitCount;
			emitter.data.NextEmitCount = 0;

			if (emitter.data.TotalEmitCount >= paramSet.Basic.EmitCount)
			{
				emitter.SetEmitting(false);
				emitter.data.TimeStopped = emitter.data.TimeCount;
			}

			if (emitter.IsEmitting())
			{
				if (emitter.data.TimeCount >= paramSet.Basic.EmitOffset)
				{
					emitter.data.NextEmitCount = (uint32_t)(round(paramSet.Basic.EmitPerFrame * emitter.data.DeltaTime));

					if (paramSet.Basic.EmitCount >= 0)
					{
						emitter.data.NextEmitCount = std::clamp((int32_t)emitter.data.NextEmitCount, 0, paramSet.Basic.EmitCount - (int32_t)emitter.data.TotalEmitCount);
					}
				}

				if (paramSet.EmitShape.Type == EmitShape::Model)
				{
					if (!emitter.resource->emitPoints)
					{
						if (auto model = emitter.resource->effect->GetModel(paramSet.EmitShape.Model.Index))
						{
							auto points = GeneratePointCache(model, 16 * 1024, 1);
							emitter.resource->emitPointCount = (uint32_t)points.size();
							emitter.resource->emitPoints = graphics->CreateComputeBuffer(
								(int32_t)points.size(), (int32_t)sizeof(EmitPoint), points.data(), true);
						}
					}
					emitter.data.EmitPointCount = emitter.resource->emitPointCount;
				}
			}

			graphics->UpdateUniformBuffer(emitter.buffer, sizeof(EmitterData), 0, &emitter.data);

			if (emitter.data.NextEmitCount > 0)
			{
				ComputeCommand command;
				command.PipelineStatePtr = m_pipelineParticleSpawn;

				command.UniformBufferPtrs[0] = emitter.resource->paramBuffer;
				command.UniformBufferPtrs[1] = emitter.buffer;
				command.SetComputeBuffer(0, m_cbufParticles, false);
				command.SetComputeBuffer(1, (emitter.resource->emitPoints) ? emitter.resource->emitPoints : m_dummyEmitPoints, true);

				command.GroupCount = { (int32_t)emitter.data.NextEmitCount, 1, 1 };
				command.ThreadCount = { 1, 1, 1 };
				graphics->Dispatch(command);
			}
			else if (emitter.data.TimeCount >= paramSet.Basic.EmitOffset && GetEmitterParticleCount(emitter, paramSet) == 0)
			{
				FreeEmitter(emitterID);
			}
		}
	}

	// Update particles
	for (EmitterID emitterID = 0; emitterID < (EmitterID)m_emitters.size(); emitterID++)
	{
		auto& emitter = m_emitters[emitterID];
		if (emitter.IsAlive())
		{
			auto& paramSet = emitter.resource->paramSet;

			ComputeCommand command;
			command.PipelineStatePtr = m_pipelineParticleUpdate;

			command.UniformBufferPtrs[0] = emitter.resource->paramBuffer;
			command.UniformBufferPtrs[1] = emitter.buffer;
			command.SetComputeBuffer(0, m_cbufParticles, false);
			command.SetComputeBuffer(1, m_cbufTrails, false);

			command.SetTexture(2, (emitter.resource->noiseTexture) ? emitter.resource->noiseTexture : m_dummyVectorTexture,
				Effekseer::Backend::TextureWrapType::Repeat, Effekseer::Backend::TextureSamplingType::Linear);
			command.SetTexture(3, (emitter.resource->fieldTexture) ? emitter.resource->fieldTexture : m_dummyVectorTexture,
				Effekseer::Backend::TextureWrapType::Repeat, Effekseer::Backend::TextureSamplingType::Linear);
			command.SetTexture(4, (emitter.resource->gradientTexture) ? emitter.resource->gradientTexture : m_dummyColorTexture,
				Effekseer::Backend::TextureWrapType::Clamp, Effekseer::Backend::TextureSamplingType::Linear);

			command.GroupCount = { (int32_t)emitter.data.ParticleSize / 256, 1, 1 };
			command.ThreadCount = { 256, 1, 1 };
			graphics->Dispatch(command);

			if (emitter.data.TrailSize > 0)
			{
				emitter.data.TrailPhase = (emitter.data.TrailPhase + 1) % paramSet.RenderShape.Data;
			}
		}
	}
}

void GpuParticles::RenderFrame()
{
	auto renderer = m_rendererBase;
	auto graphics = renderer->GetGraphicsDevice();

	// Update constant buffer
	DrawConstants cdata{};
	cdata.ProjMat = renderer->GetProjectionMatrix();
	cdata.CameraMat = renderer->GetCameraMatrix();

	Effekseer::Matrix44	inv{};
	Effekseer::Matrix44::Inverse(inv, cdata.CameraMat);
	cdata.BillboardMat = {
		float4{ inv.Values[0][0], inv.Values[1][0], inv.Values[2][0], 0.0f },
		float4{ inv.Values[0][1], inv.Values[1][1], inv.Values[2][1], 0.0f },
		float4{ inv.Values[0][2], inv.Values[1][2], inv.Values[2][2], 0.0f }
	};
	cdata.YAxisFixedMat = {
		float4{ inv.Values[0][0], 0.0f, inv.Values[2][0], 0.0f },
		float4{ inv.Values[0][1], 1.0f, inv.Values[2][1], 0.0f },
		float4{ inv.Values[0][2], 0.0f, inv.Values[2][2], 0.0f }
	};
	auto normalize = [](Effekseer::Vector3D v) {
		Effekseer::Vector3D::Normal(v, v);
		return v;
	};
	cdata.CameraPos = renderer->GetCameraPosition();
	cdata.CameraFront = normalize(renderer->GetCameraFrontDirection());
	cdata.LightDir = normalize(renderer->GetLightDirection());
	cdata.LightColor = renderer->GetLightColor().ToFloat4();
	cdata.LightAmbient = renderer->GetLightAmbientColor().ToFloat4();
	graphics->UpdateUniformBuffer(m_ubufConstants, sizeof(DrawConstants), 0, &cdata);

	auto toSamplingType = [](uint8_t filterType){
		return (static_cast<Effekseer::TextureFilterType>(filterType) == Effekseer::TextureFilterType::Linear) ? Effekseer::Backend::TextureSamplingType::Linear : Effekseer::Backend::TextureSamplingType::Nearest;
	};
	auto toWrapType = [](uint8_t wrapType){
		return (static_cast<Effekseer::TextureWrapType>(wrapType) == Effekseer::TextureWrapType::Repeat) ? Effekseer::Backend::TextureWrapType::Repeat : Effekseer::Backend::TextureWrapType::Clamp;
	};

	for (EmitterID emitterID = 0; emitterID < (EmitterID)m_emitters.size(); emitterID++)
	{
		auto& emitter = m_emitters[emitterID];
		if (emitter.IsAlive())
		{
			auto effect = emitter.resource->effect;
			auto& paramSet = emitter.resource->paramSet;

			Effekseer::Backend::DrawParameter drawParams;
			drawParams.PipelineStatePtr = emitter.resource->piplineState;

			drawParams.VertexUniformBufferPtrs[0] = drawParams.PixelUniformBufferPtrs[0] = m_ubufConstants;
			drawParams.VertexUniformBufferPtrs[1] = drawParams.PixelUniformBufferPtrs[1] = emitter.resource->paramBuffer;
			drawParams.VertexUniformBufferPtrs[2] = emitter.buffer;

			drawParams.SetComputeBuffer(0, m_cbufParticles);
			drawParams.SetComputeBuffer(1, m_cbufTrails);

			auto setTexture = [&](int32_t slot, Effekseer::TextureRef texture, TextureRef defaultTexture)
			{
				drawParams.SetTexture(2 + slot, texture ? texture->GetBackend() : defaultTexture, 
					toWrapType(paramSet.RenderMaterial.TextureWraps[slot]), 
					toSamplingType(paramSet.RenderMaterial.TextureFilters[slot]));
			};

			if (paramSet.RenderMaterial.Material == MaterialType::Unlit)
			{
				auto colorTexture = effect->GetColorImage(paramSet.RenderMaterial.TextureIndexes[0]);
				setTexture(0, colorTexture, m_dummyColorTexture);
				setTexture(1, nullptr, m_dummyNormalTexture);
			}
			else if (paramSet.RenderMaterial.Material == MaterialType::Lighting)
			{
				auto colorTexture = effect->GetColorImage(paramSet.RenderMaterial.TextureIndexes[0]);
				auto normalTexture = effect->GetNormalImage(paramSet.RenderMaterial.TextureIndexes[1]);
				setTexture(0, colorTexture, m_dummyColorTexture);
				setTexture(1, normalTexture, m_dummyNormalTexture);
			}
			else
			{
				for (int32_t slot = 0; slot < 4; slot++)
				{
					auto texture = effect->GetColorImage(paramSet.RenderMaterial.TextureIndexes[slot]);
					setTexture(slot, texture, m_dummyColorTexture);
				}
			}

			Effekseer::ModelRef model;
			switch (paramSet.RenderShape.Type)
			{
				case RenderShape::Sprite: model = m_modelSprite; break;
				case RenderShape::Model: model = effect->GetModel(paramSet.RenderShape.Data); break;
				case RenderShape::Trail: model = m_modelTrail; break;
			}

			if (model)
			{
				if (!model->GetIsBufferStoredOnGPU())
				{
					model->StoreBufferToGPU(graphics.Get());
				}

				int32_t modelFrameCount = model->GetFrameCount();
				for (int32_t i = 0; i < modelFrameCount; i++)
				{
					drawParams.VertexBufferPtr = model->GetVertexBuffer(i);
					drawParams.VertexStride = sizeof(Effekseer::Model::Vertex);
					drawParams.IndexBufferPtr = model->GetIndexBuffer(i);

					if (emitter.data.TrailSize > 0)
					{
						drawParams.PrimitiveCount = paramSet.RenderShape.Data * 2;
					}
					else
					{
						drawParams.PrimitiveCount = model->GetFaceCount(i);
					}
					drawParams.InstanceCount = emitter.data.ParticleSize;
					
					graphics->Draw(drawParams);
				}
			}
		}
	}
}

GpuParticles::ResourceRef GpuParticles::CreateResource(const ParamSet& paramSet, const Effekseer::Effect* effect)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	auto graphics = m_rendererBase->GetGraphicsDevice();

	auto resource = Effekseer::MakeRefPtr<Resource>();
	resource->paramSet = paramSet;
	resource->effect = effect;
	resource->piplineState = CreatePiplineState(paramSet);

	if (paramSet.Force.TurbulencePower != 0.0f)
	{
		Effekseer::LightCurlNoise noise(paramSet.Force.TurbulenceSeed, paramSet.Force.TurbulenceScale, paramSet.Force.TurbulenceOctave);
		
		Effekseer::Backend::TextureParameter texParam;
		texParam.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		texParam.Size = { 8, 8, 8 };
		texParam.Dimension = 3;

		Effekseer::CustomVector<uint8_t> initialData;
		initialData.resize(sizeof(uint32_t) * 8 * 8 * 8);
		memcpy(initialData.data(), noise.VectorField(), initialData.size());

		resource->noiseTexture = graphics->CreateTexture(texParam, initialData);
	}

	if (paramSet.RenderColor.ColorAllType == ColorParamType::FCurve || paramSet.RenderColor.ColorAllType == ColorParamType::Gradient)
	{
		Effekseer::Backend::TextureParameter texParam;
		texParam.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		texParam.Size = { 32, 1, 1 };
		texParam.Dimension = 2;

		Effekseer::CustomVector<uint8_t> initialData;
		auto& gradient = paramSet.RenderColor.ColorAll.Gradient;
		initialData.resize(gradient.Pixels.size() * sizeof(uint32_t));
		memcpy(initialData.data(), gradient.Pixels.data(), initialData.size());

		resource->gradientTexture = graphics->CreateTexture(texParam, initialData);
	}

	// Initialize parameter data
	ParameterData paramData = ToParamData(paramSet);
	resource->paramBuffer = graphics->CreateUniformBuffer(sizeof(ParameterData), &paramData);

	return resource;
}

GpuParticles::EmitterID GpuParticles::NewEmitter(ResourceRef resource, Effekseer::InstanceGlobal* instanceGlobal)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_emitterFreeList.size() == 0)
	{
		return InvalidID;
	}

	EmitterID emitterID = m_emitterFreeList.front();

	auto& paramSet = resource->GetParamSet();
	uint32_t particlesMaxCount = (uint32_t)(round((double)paramSet.Basic.LifeTime[0] * paramSet.Basic.EmitPerFrame));
	particlesMaxCount = std::min(particlesMaxCount, (uint32_t)paramSet.Basic.EmitCount);
	particlesMaxCount = RoundUp(particlesMaxCount, ParticleUnitSize);

	Emitter& emitter = m_emitters[emitterID];
	emitter.resource = resource.DownCast<Resource>();
	emitter.instanceGlobal = instanceGlobal;
	emitter.data = {};
	emitter.data.Color = 0xFFFFFFFF;
	emitter.data.Transform = {
		float4{ 1.0f, 0.0f, 0.0f, 0.0f },
		float4{ 0.0f, 1.0f, 0.0f, 0.0f },
		float4{ 0.0f, 0.0f, 1.0f, 0.0f }
	};
	emitter.SetFlagBits(true, false);

	Block particleBlock = m_particleAllocator.Allocate(particlesMaxCount);
	if (particleBlock.size == 0)
	{
		return InvalidID;
	}
	emitter.data.ParticleHead = particleBlock.offset;
	emitter.data.ParticleSize = particleBlock.size;

	if (paramSet.RenderShape.Type == RenderShape::Trail)
	{
		Block trailBlock = m_trailAllocator.Allocate(particlesMaxCount * paramSet.RenderShape.Data);
		if (trailBlock.size == 0)
		{
			m_particleAllocator.Deallocate(particleBlock);
			return InvalidID;
		}
		emitter.data.TrailHead = trailBlock.offset;
		emitter.data.TrailSize = trailBlock.size;
	}

	m_newEmitterIDs.push_back(emitterID);
	m_emitterFreeList.pop_front();

	return emitterID;
}

void GpuParticles::FreeEmitter(EmitterID emitterID)
{
	assert(emitterID >= 0 && emitterID < m_emitters.size());

	Emitter& emitter = m_emitters[emitterID];
	emitter.resource = nullptr;
	emitter.instanceGlobal = nullptr;
	emitter.data.FlagBits = 0;

	m_particleAllocator.Deallocate({ emitter.data.ParticleHead, emitter.data.ParticleSize });
	m_trailAllocator.Deallocate({ emitter.data.TrailHead, emitter.data.TrailSize });
	m_emitterFreeList.push_front(emitterID);
}

void GpuParticles::StartEmit(EmitterID emitterID)
{
	assert(emitterID >= 0 && emitterID < m_emitters.size());
	Emitter& emitter = m_emitters[emitterID];
	emitter.SetEmitting(true);
	emitter.data.TimeCount = 0.0f;
}

void GpuParticles::StopEmit(EmitterID emitterID)
{
	assert(emitterID >= 0 && emitterID < m_emitters.size());
	Emitter& emitter = m_emitters[emitterID];
	emitter.SetEmitting(false);
	emitter.data.TimeStopped = emitter.data.TimeCount;
}

void GpuParticles::SetRandomSeed(EmitterID emitterID, uint32_t seed)
{
	assert(emitterID >= 0 && emitterID < m_emitters.size());
	Emitter& emitter = m_emitters[emitterID];
	emitter.data.Seed = seed;
}

void GpuParticles::SetTransform(EmitterID emitterID, const Effekseer::Matrix43& transform)
{
	assert(emitterID >= 0 && emitterID < m_emitters.size());
	Emitter& emitter = m_emitters[emitterID];
	emitter.data.Transform = {
		float4{ transform.Value[0][0], transform.Value[1][0], transform.Value[2][0], transform.Value[3][0] },
		float4{ transform.Value[0][1], transform.Value[1][1], transform.Value[2][1], transform.Value[3][1] },
		float4{ transform.Value[0][2], transform.Value[1][2], transform.Value[2][2], transform.Value[3][2] }
	};
}

void GpuParticles::SetColor(EmitterID emitterID, Effekseer::Color color)
{
	assert(emitterID >= 0 && emitterID < m_emitters.size());
	Emitter& emitter = m_emitters[emitterID];
	emitter.data.Color = *reinterpret_cast<uint32_t*>(&color);
}

void GpuParticles::SetDeltaTime(Effekseer::InstanceGlobal* instanceGlobal, float deltaTime)
{
	for (EmitterID emitterID = 0; emitterID < (EmitterID)m_emitters.size(); emitterID++)
	{
		auto& emitter = m_emitters[emitterID];
		if (emitter.IsAlive() && emitter.instanceGlobal == instanceGlobal)
		{
			emitter.data.DeltaTime = deltaTime;
		}
	}
}

void GpuParticles::KillParticles(Effekseer::InstanceGlobal* instanceGlobal)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	for (EmitterID emitterID = 0; emitterID < (EmitterID)m_emitters.size(); emitterID++)
	{
		auto& emitter = m_emitters[emitterID];
		if (emitter.IsAlive() && emitter.instanceGlobal == instanceGlobal)
		{
			FreeEmitter(emitterID);
		}
	}
}

int32_t GpuParticles::GetParticleCount(Effekseer::InstanceGlobal* instanceGlobal)
{
	int32_t count = 0;

	for (EmitterID emitterID = 0; emitterID < (EmitterID)m_emitters.size(); emitterID++)
	{
		auto& emitter = m_emitters[emitterID];
		if (emitter.IsAlive() && emitter.instanceGlobal == instanceGlobal)
		{
			count += GetEmitterParticleCount(emitter, emitter.resource->paramSet);
		}
	}
	return count;
}

GpuParticles::ParameterData GpuParticles::ToParamData(const ParamSet& paramSet)
{
	ParameterData data{};
	data.LifeTime = paramSet.Basic.LifeTime;
	data.EmitShapeType = (uint)paramSet.EmitShape.Type;
	data.EmitRotationApplied = (uint)paramSet.EmitShape.RotationApplied;

	data.EmitShapeData[0] = paramSet.EmitShape.Data[0];
	data.EmitShapeData[1] = paramSet.EmitShape.Data[1];

	data.Direction = paramSet.Position.Direction;
	data.Spread = paramSet.Position.Spread;
	data.InitialSpeed = paramSet.Position.InitialSpeed;
	data.Damping = paramSet.Position.Damping;

	data.AngularOffset[0] = float4(paramSet.Rotation.Offset[0], 0.0f);
	data.AngularOffset[1] = float4(paramSet.Rotation.Offset[1], 0.0f);
	data.AngularVelocity[0] = float4(paramSet.Rotation.Velocity[0], 0.0f);
	data.AngularVelocity[1] = float4(paramSet.Rotation.Velocity[1], 0.0f);

	data.ScaleData1[0] = paramSet.Scale.Data[0];
	data.ScaleData1[1] = paramSet.Scale.Data[1];
	data.ScaleData2[0] = paramSet.Scale.Data[2];
	data.ScaleData2[1] = paramSet.Scale.Data[3];
	data.ScaleEasing = paramSet.Scale.Easing.Speed;
	data.ScaleFlags = (uint)paramSet.Scale.Type;

	data.Gravity = paramSet.Force.Gravity;

	data.VortexCenter = paramSet.Force.VortexCenter;
	data.VortexRotation = paramSet.Force.VortexRotation;
	data.VortexAxis = paramSet.Force.VortexAxis;
	data.VortexAttraction = paramSet.Force.VortexAttraction;

	data.TurbulencePower = paramSet.Force.TurbulencePower;
	data.TurbulenceSeed = paramSet.Force.TurbulenceSeed;
	data.TurbulenceScale = paramSet.Force.TurbulenceScale;
	data.TurbulenceOctave = paramSet.Force.TurbulenceOctave;

	data.ShapeType = (uint)paramSet.RenderShape.Type;
	data.ShapeData = paramSet.RenderShape.Data;
	data.ShapeSize = paramSet.RenderShape.Size;

	data.Emissive = paramSet.RenderColor.Emissive;
	data.FadeIn = paramSet.RenderColor.FadeIn;
	data.FadeOut = paramSet.RenderColor.FadeOut;
	data.MaterialType = (uint)paramSet.RenderMaterial.Material;

	data.ColorData = paramSet.RenderColor.ColorAll.Data;
	data.ColorEasing = paramSet.RenderColor.ColorAll.Easing.Speed;
	data.ColorFlags = (uint)paramSet.RenderColor.ColorAllType | ((uint)paramSet.RenderColor.ColorInherit << 3) | ((uint)paramSet.RenderColor.ColorSpace << 5);

	return data;
}

int32_t GpuParticles::GetEmitterParticleCount(const Emitter& emitter, const ParamSet& paramSet)
{
	int32_t maxParticleCount = static_cast<int32_t>(paramSet.Basic.LifeTime[0] * paramSet.Basic.EmitPerFrame);
	float emitDuration = static_cast<float>(paramSet.Basic.EmitCount) / static_cast<float>(paramSet.Basic.EmitPerFrame);
	float timeCount = std::max(0.0f, emitter.data.TimeCount - paramSet.Basic.EmitOffset);

	if (!emitter.IsEmitting())
	{
		emitDuration = std::min(emitDuration, emitter.data.TimeStopped - paramSet.Basic.EmitOffset);
	}
	if (timeCount < paramSet.Basic.LifeTime[0])
	{
		return static_cast<int32_t>(paramSet.Basic.EmitPerFrame * timeCount);
	}
	else if (timeCount < emitDuration)
	{
		return maxParticleCount;
	}
	else
	{
		return std::max(0, maxParticleCount - static_cast<int32_t>(paramSet.Basic.EmitPerFrame * (timeCount - emitDuration)));
	}
}

GpuParticles::PipelineStateRef GpuParticles::CreatePiplineState(const ParamSet& paramSet)
{
	using namespace Effekseer::Backend;

	PipelineStateParameter pipParams;
	if ((Effekseer::AlphaBlendType)paramSet.RenderBasic.BlendType == Effekseer::AlphaBlendType::Opacity)
	{
		pipParams.IsBlendEnabled = false;
	}
	else
	{
		pipParams.IsBlendEnabled = true;
		switch ((Effekseer::AlphaBlendType)paramSet.RenderBasic.BlendType)
		{
		case Effekseer::AlphaBlendType::Opacity:
			pipParams.BlendEquationRGB = BlendEquationType::Add;
			pipParams.BlendSrcFunc = BlendFuncType::One;
			pipParams.BlendDstFunc = BlendFuncType::Zero;
		case Effekseer::AlphaBlendType::Blend:
			pipParams.BlendEquationRGB = BlendEquationType::Add;
			pipParams.BlendSrcFunc = BlendFuncType::SrcAlpha;
			pipParams.BlendDstFunc = BlendFuncType::OneMinusSrcAlpha;
			break;
		case Effekseer::AlphaBlendType::Add:
			pipParams.BlendEquationRGB = BlendEquationType::Add;
			pipParams.BlendSrcFunc = BlendFuncType::SrcAlpha;
			pipParams.BlendDstFunc = BlendFuncType::One;
			break;
		case Effekseer::AlphaBlendType::Sub:
			pipParams.BlendDstFunc = BlendFuncType::One;
			pipParams.BlendSrcFunc = BlendFuncType::SrcAlpha;
			pipParams.BlendEquationRGB = BlendEquationType::ReverseSub;
			pipParams.BlendSrcFuncAlpha = BlendFuncType::Zero;
			pipParams.BlendDstFuncAlpha = BlendFuncType::One;
			pipParams.BlendEquationAlpha = BlendEquationType::Add;
			break;
		case Effekseer::AlphaBlendType::Mul:
			pipParams.BlendDstFunc = BlendFuncType::SrcColor;
			pipParams.BlendSrcFunc = BlendFuncType::Zero;
			pipParams.BlendEquationRGB = BlendEquationType::Add;
			pipParams.BlendSrcFuncAlpha = BlendFuncType::Zero;
			pipParams.BlendDstFuncAlpha = BlendFuncType::One;
			pipParams.BlendEquationAlpha = BlendEquationType::Add;
			break;
		}
	}
	pipParams.IsDepthTestEnabled = paramSet.RenderBasic.ZTest;
	pipParams.IsDepthWriteEnabled = paramSet.RenderBasic.ZWrite;
	pipParams.Culling = CullingType::DoubleSide;
	pipParams.ShaderPtr = m_shaders.rsParticleRender;
	pipParams.VertexLayoutPtr = m_vertexLayout;

	return m_rendererBase->GetGraphicsDevice()->CreatePipelineState(pipParams);
}

}

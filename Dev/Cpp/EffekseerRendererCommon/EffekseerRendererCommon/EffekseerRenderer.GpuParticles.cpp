#include "EffekseerRenderer.GpuParticles.h"
#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.Renderer_Impl.h"
#include "../Effekseer/Effekseer/Noise/CurlNoise.h"
#include "../Effekseer/Effekseer/Model/PointCacheGenerator.h"

namespace EffekseerRenderer
{

namespace
{

inline constexpr uint32_t RoundUp(uint32_t x, uint32_t unit)
{
	return (x + unit - 1) / unit * unit;
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

}

GpuParticleFactory::GpuParticleFactory(Effekseer::Backend::GraphicsDeviceRef graphics)
	: m_graphics(graphics)
{
}

Effekseer::GpuParticles::ResourceRef GpuParticleFactory::CreateResource(const Effekseer::GpuParticles::ParamSet& paramSet, const Effekseer::Effect* effect)
{
	using namespace Effekseer::GpuParticles;

	auto resource = Effekseer::MakeRefPtr<GpuParticles::Resource>();
	resource->paramSet = paramSet;
	resource->effect = effect;
	resource->piplineStateKey = ToPiplineStateKey(paramSet);

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

		resource->noiseTexture = m_graphics->CreateTexture(texParam, initialData);
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

		resource->gradientTexture = m_graphics->CreateTexture(texParam, initialData);
	}

	// Initialize parameter data
	GpuParticles::ParameterData paramData = ToParamData(paramSet);
	resource->paramBuffer = m_graphics->CreateUniformBuffer(sizeof(GpuParticles::ParameterData), &paramData);

	return resource;
}

GpuParticles::ParameterData GpuParticleFactory::ToParamData(const Effekseer::GpuParticles::ParamSet& paramSet)
{
	using namespace Effekseer::GpuParticles;

	GpuParticles::ParameterData data{};
	data.LifeTime = paramSet.Basic.LifeTime;
	data.EmitShapeType = (uint)paramSet.EmitShape.Type;
	data.EmitRotationApplied = (uint)paramSet.EmitShape.RotationApplied;

	data.EmitShapeData[0] = paramSet.EmitShape.Data[0];
	data.EmitShapeData[1] = paramSet.EmitShape.Data[1];

	data.Direction = paramSet.Velocity.Direction;
	data.Spread = paramSet.Velocity.Spread;
	data.InitialSpeed = paramSet.Velocity.InitialSpeed;
	data.Damping = paramSet.Velocity.Damping;

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

GpuParticles::PipelineStateKey GpuParticleFactory::ToPiplineStateKey(const Effekseer::GpuParticles::ParamSet& paramSet)
{
	GpuParticles::PipelineStateKey key;
	key.BlendType = (uint8_t)paramSet.RenderBasic.BlendType;
	key.CullingType = (uint8_t)Effekseer::CullingType::Double;
	key.ZTest = paramSet.RenderBasic.ZTest;
	key.ZWrite = paramSet.RenderBasic.ZWrite;
	return key;
}

void GpuParticleSystem::BlockAllocator::Init(uint32_t bufferSize, uint32_t blockSize)
{
	bufferBlocks.reserve(bufferSize / blockSize);
	bufferBlocks.push_back({0, bufferSize});
}

GpuParticleSystem::Block GpuParticleSystem::BlockAllocator::Allocate(uint32_t size)
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

void GpuParticleSystem::BlockAllocator::Deallocate(Block releasingBlock)
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

GpuParticleSystem::GpuParticleSystem(Renderer* renderer)
	: m_rendererBase(renderer)
{
}

GpuParticleSystem::~GpuParticleSystem()
{
}

bool GpuParticleSystem::InitSystem(const Settings& settings)
{
	auto graphics = m_rendererBase->GetGraphicsDevice();

	m_settings = settings;
	m_emitters.resize(settings.EmitterMaxCount);

	for (uint32_t index = 0; index < settings.EmitterMaxCount; index++)
	{
		m_emitterFreeList.push_back(index);
		m_emitters[index].buffer = graphics->CreateUniformBuffer(sizeof(GpuParticles::EmitterData), nullptr);
	}
	m_newEmitterIDs.reserve(settings.EmitterMaxCount);

	m_particleAllocator.Init(settings.ParticleMaxCount, ParticleUnitSize);
	m_trailAllocator.Init(settings.TrailMaxCount, ParticleUnitSize);

	GpuParticles::ComputeConstants computeConstants{};
	m_ubufComputeConstants = graphics->CreateUniformBuffer(sizeof(GpuParticles::ComputeConstants), &computeConstants);

	GpuParticles::RenderConstants renderConstants{};
	m_ubufRenderConstants = graphics->CreateUniformBuffer(sizeof(GpuParticles::RenderConstants), &renderConstants);
	
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
		GpuParticles::EmitPoint dummyData = {};
		m_dummyEmitPoints = graphics->CreateComputeBuffer(1, sizeof(dummyData), &dummyData, true);
	}

	m_dummyColorTexture = m_rendererBase->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
	m_dummyNormalTexture = m_rendererBase->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);

	return true;
}

void GpuParticleSystem::SetShaders(const Shaders& shaders)
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

void GpuParticleSystem::ComputeFrame(const Context& context)
{
	using namespace Effekseer::GpuParticles;

	auto renderer = m_rendererBase;
	auto graphics = renderer->GetGraphicsDevice();

	{
		GpuParticles::ComputeConstants cdata{};
		cdata.CoordinateReversed = context.CoordinateReversed;
		graphics->UpdateUniformBuffer(m_ubufComputeConstants, sizeof(GpuParticles::ComputeConstants), 0, &cdata);
	}

	for (auto emitterID : m_newEmitterIDs)
	{
		// Initialize particle data region
		auto& emitter = m_emitters[emitterID];
		graphics->UpdateUniformBuffer(emitter.buffer, sizeof(GpuParticles::EmitterData), 0, &emitter.data);

		GpuParticles::ComputeCommand command;
		command.PipelineStatePtr = m_pipelineParticleClear;

		command.UniformBufferPtrs[0] = m_ubufComputeConstants;
		command.UniformBufferPtrs[1] = emitter.resource->paramBuffer;
		command.UniformBufferPtrs[2] = emitter.buffer;
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

				if (paramSet.EmitShape.Type == EmitShapeT::Model)
				{
					if (!emitter.resource->emitPoints)
					{
						if (auto model = emitter.resource->effect->GetModel(paramSet.EmitShape.Model.Index))
						{
							const uint32_t PointCount = 16 * 1024;
							Effekseer::CustomVector<GpuParticles::EmitPoint> points;
							points.resize(PointCount);

							Effekseer::PointCacheGenerator pcgen;
							pcgen.SetSourceModel(model);
							pcgen.SetPointBuffer(&points[0].Position, sizeof(GpuParticles::EmitPoint));
							pcgen.SetAttributeBuffer(&points[0].Normal, sizeof(GpuParticles::EmitPoint));
							pcgen.Generate(PointCount, 1);

							emitter.resource->emitPointCount = (uint32_t)points.size();
							emitter.resource->emitPoints = graphics->CreateComputeBuffer(
								(int32_t)points.size(), (int32_t)sizeof(GpuParticles::EmitPoint), points.data(), true);
						}
					}
					emitter.data.EmitPointCount = emitter.resource->emitPointCount;
				}
			}

			graphics->UpdateUniformBuffer(emitter.buffer, sizeof(GpuParticles::EmitterData), 0, &emitter.data);

			if (emitter.data.NextEmitCount > 0)
			{
				GpuParticles::ComputeCommand command;
				command.PipelineStatePtr = m_pipelineParticleSpawn;

				command.UniformBufferPtrs[0] = m_ubufComputeConstants;
				command.UniformBufferPtrs[1] = emitter.resource->paramBuffer;
				command.UniformBufferPtrs[2] = emitter.buffer;
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

			GpuParticles::ComputeCommand command;
			command.PipelineStatePtr = m_pipelineParticleUpdate;

			command.UniformBufferPtrs[0] = m_ubufComputeConstants;
			command.UniformBufferPtrs[1] = emitter.resource->paramBuffer;
			command.UniformBufferPtrs[2] = emitter.buffer;
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

void GpuParticleSystem::RenderFrame(const Context& context)
{
	using namespace Effekseer::GpuParticles;

	auto renderer = m_rendererBase;
	auto graphics = renderer->GetGraphicsDevice();

	// Update constant buffer
	{
		GpuParticles::RenderConstants cdata{};
		cdata.CoordinateReversed = context.CoordinateReversed;
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
		graphics->UpdateUniformBuffer(m_ubufRenderConstants, sizeof(GpuParticles::RenderConstants), 0, &cdata);
	}

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
			drawParams.PipelineStatePtr = GetOrCreatePipelineState(emitter.resource->piplineStateKey);

			drawParams.VertexUniformBufferPtrs[0] = drawParams.PixelUniformBufferPtrs[0] = m_ubufRenderConstants;
			drawParams.VertexUniformBufferPtrs[1] = drawParams.PixelUniformBufferPtrs[1] = emitter.resource->paramBuffer;
			drawParams.VertexUniformBufferPtrs[2] = emitter.buffer;

			drawParams.SetComputeBuffer(0, m_cbufParticles);
			drawParams.SetComputeBuffer(1, m_cbufTrails);

			auto setTexture = [&](int32_t slot, Effekseer::TextureRef texture, GpuParticles::TextureRef defaultTexture)
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
				case RenderShapeT::Sprite: model = m_modelSprite; break;
				case RenderShapeT::Model: model = effect->GetModel(paramSet.RenderShape.Data); break;
				case RenderShapeT::Trail: model = m_modelTrail; break;
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

GpuParticleSystem::EmitterID GpuParticleSystem::NewEmitter(Effekseer::GpuParticles::ResourceRef resource, Effekseer::InstanceGlobal* instanceGlobal)
{
	using namespace Effekseer::GpuParticles;

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
	emitter.resource = resource.DownCast<GpuParticles::Resource>();
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

	if (paramSet.RenderShape.Type == RenderShapeT::Trail)
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

void GpuParticleSystem::FreeEmitter(EmitterID emitterID)
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

void GpuParticleSystem::StartEmit(EmitterID emitterID)
{
	assert(emitterID >= 0 && emitterID < m_emitters.size());
	Emitter& emitter = m_emitters[emitterID];
	emitter.SetEmitting(true);
	emitter.data.TimeCount = 0.0f;
}

void GpuParticleSystem::StopEmit(EmitterID emitterID)
{
	assert(emitterID >= 0 && emitterID < m_emitters.size());
	Emitter& emitter = m_emitters[emitterID];
	emitter.SetEmitting(false);
	emitter.data.TimeStopped = emitter.data.TimeCount;
}

void GpuParticleSystem::SetRandomSeed(EmitterID emitterID, uint32_t seed)
{
	assert(emitterID >= 0 && emitterID < m_emitters.size());
	Emitter& emitter = m_emitters[emitterID];
	emitter.data.Seed = seed;
}

void GpuParticleSystem::SetTransform(EmitterID emitterID, const Effekseer::Matrix43& transform)
{
	using namespace Effekseer::GpuParticles;

	assert(emitterID >= 0 && emitterID < m_emitters.size());
	Emitter& emitter = m_emitters[emitterID];
	emitter.data.Transform = {
		float4{ transform.Value[0][0], transform.Value[1][0], transform.Value[2][0], transform.Value[3][0] },
		float4{ transform.Value[0][1], transform.Value[1][1], transform.Value[2][1], transform.Value[3][1] },
		float4{ transform.Value[0][2], transform.Value[1][2], transform.Value[2][2], transform.Value[3][2] }
	};
}

void GpuParticleSystem::SetColor(EmitterID emitterID, Effekseer::Color color)
{
	assert(emitterID >= 0 && emitterID < m_emitters.size());
	Emitter& emitter = m_emitters[emitterID];
	emitter.data.Color = *reinterpret_cast<uint32_t*>(&color);
}

void GpuParticleSystem::SetDeltaTime(Effekseer::InstanceGlobal* instanceGlobal, float deltaTime)
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

void GpuParticleSystem::KillParticles(Effekseer::InstanceGlobal* instanceGlobal)
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

int32_t GpuParticleSystem::GetParticleCount(Effekseer::InstanceGlobal* instanceGlobal)
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

int32_t GpuParticleSystem::GetEmitterParticleCount(const Emitter& emitter, const Effekseer::GpuParticles::ParamSet& paramSet)
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

GpuParticles::PipelineStateRef GpuParticleSystem::GetOrCreatePipelineState(GpuParticles::PipelineStateKey key)
{
	using namespace Effekseer::Backend;

	for (auto& [cachedKey, cachedPipeline] : m_pipelineParticleRenders)
	{
		if (cachedKey.Data == key.Data)
		{
			return cachedPipeline;
		}
	}

	PipelineStateParameter pipParams;
	if ((Effekseer::AlphaBlendType)key.BlendType == Effekseer::AlphaBlendType::Opacity)
	{
		pipParams.IsBlendEnabled = false;
	}
	else
	{
		pipParams.IsBlendEnabled = true;
		switch ((Effekseer::AlphaBlendType)key.BlendType)
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

	switch ((Effekseer::CullingType)key.CullingType)
	{
	case Effekseer::CullingType::Front:
		pipParams.Culling = CullingType::Clockwise;
		break;
	case Effekseer::CullingType::Back:
		pipParams.Culling = CullingType::CounterClockwise;
		break;
	case Effekseer::CullingType::Double:
		pipParams.Culling = CullingType::DoubleSide;
		break;
	}

	pipParams.IsDepthTestEnabled = key.ZTest;
	pipParams.IsDepthWriteEnabled = key.ZWrite;
	pipParams.ShaderPtr = m_shaders.rsParticleRender;
	pipParams.VertexLayoutPtr = m_vertexLayout;

	GpuParticles::PipelineStateRef pipeline = m_rendererBase->GetGraphicsDevice()->CreatePipelineState(pipParams);

	m_pipelineParticleRenders.emplace_back(key, pipeline);

	return pipeline;
}

}

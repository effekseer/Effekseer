#include "EffekseerRenderer.GpuParticles.h"
#include "../Effekseer/Effekseer/Model/Effekseer.PointCacheGenerator.h"
#include "../Effekseer/Effekseer/Noise/Effekseer.CurlNoise.h"
#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.Renderer_Impl.h"

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
		{{-0.5f, 0.5f, 0.0f}, VEC_FRONT, VEC_UP, VEC_RIGHT, {0.0f, 0.0f}, {0.0f, 0.0f}, COLOR_WHITE},
		{{-0.5f, -0.5f, 0.0f}, VEC_FRONT, VEC_UP, VEC_RIGHT, {0.0f, 1.0f}, {0.0f, 1.0f}, COLOR_WHITE},
		{{0.5f, 0.5f, 0.0f}, VEC_FRONT, VEC_UP, VEC_RIGHT, {1.0f, 0.0f}, {1.0f, 0.0f}, COLOR_WHITE},
		{{0.5f, -0.5f, 0.0f}, VEC_FRONT, VEC_UP, VEC_RIGHT, {1.0f, 1.0f}, {1.0f, 1.0f}, COLOR_WHITE},
	};
	Effekseer::CustomVector<Effekseer::Model::Face> faces = {
		{{0, 2, 1}}, {{1, 2, 3}}};
	return Effekseer::MakeRefPtr<Effekseer::Model>(vertexes, faces);
}

Effekseer::ModelRef CreateTrailModel()
{
	const size_t TrailJoints = 256;
	Effekseer::CustomVector<Effekseer::Model::Vertex> vertexes;

	vertexes.resize(TrailJoints * 2);
	for (size_t i = 0; i < TrailJoints * 2; i++)
	{
		Effekseer::Model::Vertex v{};
		v.Position.X = (i % 2 == 0) ? -0.5f : 0.5f;
		v.Position.Y = 0.0f;
		v.Position.Z = 0.0f;
		v.Binormal = VEC_FRONT;
		v.Normal = VEC_UP;
		v.Tangent = VEC_RIGHT;
		v.UV1.X = v.UV2.X = (i % 2 == 0) ? 0.0f : 1.0f;
		v.UV1.Y = v.UV2.Y = (float)(i / 2) / (float)(TrailJoints - 1);
		v.VColor = COLOR_WHITE;
		vertexes[i] = v;
	}

	Effekseer::CustomVector<Effekseer::Model::Face> faces;
	faces.resize((TrailJoints - 1) * 2);
	for (size_t i = 0; i < TrailJoints - 1; i++)
	{
		int32_t ofs = (int32_t)(2 * i);
		faces[i * 2 + 0] = Effekseer::Model::Face{{ofs + 0, ofs + 2, ofs + 1}};
		faces[i * 2 + 1] = Effekseer::Model::Face{{ofs + 1, ofs + 2, ofs + 3}};
	}
	return Effekseer::MakeRefPtr<Effekseer::Model>(vertexes, faces);
}

} // namespace

GpuParticleFactory::GpuParticleFactory(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
}

Effekseer::GpuParticles::ResourceRef GpuParticleFactory::CreateResource(const Effekseer::GpuParticles::ParamSet& paramSet, const Effekseer::Effect* effect)
{
	using namespace Effekseer::GpuParticles;

	auto resource = Effekseer::MakeRefPtr<GpuParticles::Resource>();
	resource->ParamSet = paramSet;
	resource->Effect = effect;
	resource->GPUPipelineStateKey = ToPiplineStateKey(paramSet);

	if (paramSet.Force.TurbulencePower != 0.0f)
	{
		Effekseer::LightCurlNoise noise(paramSet.Force.TurbulenceSeed, paramSet.Force.TurbulenceScale, paramSet.Force.TurbulenceOctave);

		Effekseer::Backend::TextureParameter texParam;
		texParam.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		texParam.Size = {8, 8, 8};
		texParam.Dimension = 3;

		Effekseer::CustomVector<uint8_t> initialData;
		initialData.resize(sizeof(uint32_t) * 8 * 8 * 8);
		memcpy(initialData.data(), noise.VectorField(), initialData.size());

		resource->NoiseTexture = graphicsDevice_->CreateTexture(texParam, initialData);
	}

	if (paramSet.RenderColor.ColorAllType == ColorParamType::FCurve || paramSet.RenderColor.ColorAllType == ColorParamType::Gradient)
	{
		Effekseer::Backend::TextureParameter texParam;
		texParam.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		texParam.Size = {32, 1, 1};
		texParam.Dimension = 2;

		Effekseer::CustomVector<uint8_t> initialData;
		auto& gradient = paramSet.RenderColor.ColorAll.Gradient;
		initialData.resize(gradient.Pixels.size() * sizeof(uint32_t));
		memcpy(initialData.data(), gradient.Pixels.data(), initialData.size());

		resource->GradientTexture = graphicsDevice_->CreateTexture(texParam, initialData);
	}

	// Initialize parameter data
	GpuParticles::ParameterData paramData = ToParamData(paramSet);
	resource->ParamBuffer = graphicsDevice_->CreateUniformBuffer(sizeof(GpuParticles::ParameterData), &paramData);

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
	BufferBlocks.reserve(bufferSize / blockSize);
	BufferBlocks.push_back({0, bufferSize});
}

GpuParticleSystem::Block GpuParticleSystem::BlockAllocator::Allocate(uint32_t size)
{
	Block result{};
	for (size_t i = 0; i < BufferBlocks.size(); i++)
	{
		Block& block = BufferBlocks[i];
		if (block.Size >= size)
		{
			if (block.Size == size)
			{
				result = block;
				BufferBlocks.erase(BufferBlocks.begin() + i);
				break;
			}
			else
			{
				result = Block{block.Offset, size};
				block.Offset += size;
				break;
			}
		}
	}
	return result;
}

void GpuParticleSystem::BlockAllocator::Deallocate(Block releasingBlock)
{
	if (releasingBlock.Size == 0)
	{
		return;
	}

	Block newBlock = releasingBlock;
	for (size_t i = 0; i < BufferBlocks.size(); i++)
	{
		Block& block = BufferBlocks[i];
		if (block.Offset + block.Size == newBlock.Offset)
		{
			block.Size += newBlock.Size;

			if (i + 1 < BufferBlocks.size() && block.Offset + block.Size == BufferBlocks[i + 1].Offset)
			{
				block.Size += BufferBlocks[i + 1].Size;
				BufferBlocks.erase(BufferBlocks.begin() + i + 1);
			}
			break;
		}
		else if (newBlock.Offset + newBlock.Size == block.Offset)
		{
			block.Offset -= newBlock.Size;
			break;
		}
		else if (newBlock.Offset < block.Offset)
		{
			BufferBlocks.insert(BufferBlocks.begin() + i, newBlock);
			break;
		}
	}
}

GpuParticleSystem::GpuParticleSystem(Renderer* renderer)
	: renderer_(renderer)
{
	graphicsDevice_ = renderer_->GetGraphicsDevice();
}

GpuParticleSystem::~GpuParticleSystem()
{
}

bool GpuParticleSystem::InitSystem(const Settings& settings)
{
	m_settings = settings;
	emitters_.resize(settings.EmitterMaxCount);

	for (uint32_t index = 0; index < settings.EmitterMaxCount; index++)
	{
		emitterFreeList_.push_back(index);
		emitters_[index].Buffer = graphicsDevice_->CreateUniformBuffer(sizeof(GpuParticles::EmitterData), nullptr);
	}
	newEmitterIds_.reserve(settings.EmitterMaxCount);

	particleAllocator_.Init(settings.ParticleMaxCount, ParticleUnitSize);
	trailAllocator_.Init(settings.TrailMaxCount, ParticleUnitSize);

	GpuParticles::ComputeConstants computeConstants{};
	computeConstantsUniformBuffer_ = graphicsDevice_->CreateUniformBuffer(sizeof(GpuParticles::ComputeConstants), &computeConstants);

	GpuParticles::RenderConstants renderConstants{};
	renderConstantsUniformBuffer_ = graphicsDevice_->CreateUniformBuffer(sizeof(GpuParticles::RenderConstants), &renderConstants);

	particlesComputeBuffer_ = graphicsDevice_->CreateComputeBuffer((int32_t)settings.ParticleMaxCount, (int32_t)sizeof(Particle), nullptr, false);
	trailsComputeBuffer_ = graphicsDevice_->CreateComputeBuffer((int32_t)settings.TrailMaxCount, (int32_t)sizeof(Trail), nullptr, false);

	vertexLayout_ = EffekseerRenderer::GetModelRendererVertexLayout(graphicsDevice_);

	modelSprite_ = CreateSpriteModel();
	modelSprite_->StoreBufferToGPU(graphicsDevice_.Get());

	modelTrail_ = CreateTrailModel();
	modelTrail_->StoreBufferToGPU(graphicsDevice_.Get());

	{
		Effekseer::Backend::TextureParameter texParam{};
		texParam.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		texParam.Size = {1, 1, 1};
		texParam.Dimension = 3;
		dummyVectorTexture_ = graphicsDevice_->CreateTexture(texParam, {0, 0, 0, 0});
	}

	{
		Effekseer::Backend::TextureParameter texParam{};
		texParam.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		texParam.Size = {1, 1, 1};
		texParam.MipLevelCount = 1;
		texParam.Dimension = 2;
		dummyColorTexture_ = graphicsDevice_->CreateTexture(texParam, {255, 255, 255, 255});
	}

	{
		Effekseer::Backend::TextureParameter texParam{};
		texParam.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		texParam.Size = {1, 1, 1};
		texParam.MipLevelCount = 1;
		texParam.Dimension = 2;
		dummyNormalTexture_ = graphicsDevice_->CreateTexture(texParam, {127, 127, 255, 255});
	}

	{
		GpuParticles::EmitPoint dummyData = {};
		dummyEmitPoints_ = graphicsDevice_->CreateComputeBuffer(1, sizeof(dummyData), &dummyData, true);
	}

	return true;
}

void GpuParticleSystem::SetShaders(const Shaders& shaders)
{
	shaders_ = shaders;

	{
		Effekseer::Backend::PipelineStateParameter params{};
		params.ShaderPtr = shaders_.CsParticleClear;
		pipelineParticleClear_ = graphicsDevice_->CreatePipelineState(params);
	}
	{
		Effekseer::Backend::PipelineStateParameter params{};
		params.ShaderPtr = shaders_.CsParticleSpawn;
		pipelineParticleSpawn_ = graphicsDevice_->CreatePipelineState(params);
	}
	{
		Effekseer::Backend::PipelineStateParameter params{};
		params.ShaderPtr = shaders_.CsParticleUpdate;
		pipelineParticleUpdate_ = graphicsDevice_->CreatePipelineState(params);
	}
}

void GpuParticleSystem::ComputeFrame(const Context& context)
{
	using namespace Effekseer::GpuParticles;

	{
		GpuParticles::ComputeConstants cdata{};
		cdata.CoordinateReversed = context.CoordinateReversed;
		graphicsDevice_->UpdateUniformBuffer(computeConstantsUniformBuffer_, sizeof(GpuParticles::ComputeConstants), 0, &cdata);
	}

	for (auto emitterID : newEmitterIds_)
	{
		// Initialize particle data region
		auto& emitter = emitters_[emitterID];
		graphicsDevice_->UpdateUniformBuffer(emitter.Buffer, sizeof(GpuParticles::EmitterData), 0, &emitter.Data);

		GpuParticles::ComputeCommand command;
		command.PipelineStatePtr = pipelineParticleClear_;

		command.UniformBufferPtrs[0] = computeConstantsUniformBuffer_;
		command.UniformBufferPtrs[1] = emitter.Resource->ParamBuffer;
		command.UniformBufferPtrs[2] = emitter.Buffer;
		command.SetComputeBuffer(0, particlesComputeBuffer_, false);

		command.GroupCount = {(int32_t)emitter.Data.ParticleSize / 256, 1, 1};
		command.ThreadCount = {256, 1, 1};
		graphicsDevice_->Dispatch(command);
	}
	newEmitterIds_.clear();

	// Spawn particles
	for (EmitterID emitterID = 0; emitterID < (EmitterID)emitters_.size(); emitterID++)
	{
		auto& emitter = emitters_[emitterID];
		if (emitter.IsAlive())
		{
			auto& paramSet = emitter.Resource->ParamSet;

			emitter.Data.TimeCount += emitter.Data.DeltaTime;

			emitter.Data.TotalEmitCount += emitter.Data.NextEmitCount;
			emitter.Data.NextEmitCount = 0;

			if (static_cast<int32_t>(emitter.Data.TotalEmitCount) >= paramSet.Basic.EmitCount)
			{
				emitter.SetEmitting(false);
				emitter.Data.TimeStopped = emitter.Data.TimeCount;
			}

			if (emitter.IsEmitting())
			{
				if (emitter.Data.TimeCount >= paramSet.Basic.EmitOffset)
				{
					emitter.Data.NextEmitCount = (uint32_t)(round(paramSet.Basic.EmitPerFrame * emitter.Data.DeltaTime));

					if (paramSet.Basic.EmitCount >= 0)
					{
						emitter.Data.NextEmitCount = std::clamp((int32_t)emitter.Data.NextEmitCount, 0, paramSet.Basic.EmitCount - (int32_t)emitter.Data.TotalEmitCount);
					}
				}

				if (paramSet.EmitShape.Type == EmitShapeT::Model)
				{
					if (!emitter.Resource->EmitPoints)
					{
						if (auto model = emitter.Resource->Effect->GetModel(paramSet.EmitShape.Model.Index))
						{
							const uint32_t PointCount = 16 * 1024;
							Effekseer::CustomVector<GpuParticles::EmitPoint> points;
							points.resize(PointCount);

							Effekseer::PointCacheGenerator pcgen;
							pcgen.SetSourceModel(model);
							pcgen.SetPointBuffer(&points[0].Position, sizeof(GpuParticles::EmitPoint));
							pcgen.SetAttributeBuffer(&points[0].Normal, sizeof(GpuParticles::EmitPoint));
							pcgen.Generate(PointCount, 1);

							emitter.Resource->EmitPointCount = (uint32_t)points.size();
							emitter.Resource->EmitPoints = graphicsDevice_->CreateComputeBuffer(
								(int32_t)points.size(), (int32_t)sizeof(GpuParticles::EmitPoint), points.data(), true);
						}
					}
					emitter.Data.EmitPointCount = emitter.Resource->EmitPointCount;
				}
			}

			graphicsDevice_->UpdateUniformBuffer(emitter.Buffer, sizeof(GpuParticles::EmitterData), 0, &emitter.Data);

			if (emitter.Data.NextEmitCount > 0)
			{
				GpuParticles::ComputeCommand command;
				command.PipelineStatePtr = pipelineParticleSpawn_;

				command.UniformBufferPtrs[0] = computeConstantsUniformBuffer_;
				command.UniformBufferPtrs[1] = emitter.Resource->ParamBuffer;
				command.UniformBufferPtrs[2] = emitter.Buffer;
				command.SetComputeBuffer(0, particlesComputeBuffer_, false);
				command.SetComputeBuffer(1, (emitter.Resource->EmitPoints) ? emitter.Resource->EmitPoints : dummyEmitPoints_, true);

				command.GroupCount = {(int32_t)emitter.Data.NextEmitCount, 1, 1};
				command.ThreadCount = {1, 1, 1};
				graphicsDevice_->Dispatch(command);
			}
			else if (emitter.Data.TimeCount >= paramSet.Basic.EmitOffset && GetEmitterParticleCount(emitter, paramSet) == 0)
			{
				FreeEmitter(emitterID);
			}
		}
	}

	// Update particles
	for (EmitterID emitterID = 0; emitterID < (EmitterID)emitters_.size(); emitterID++)
	{
		auto& emitter = emitters_[emitterID];
		if (emitter.IsAlive())
		{
			auto& paramSet = emitter.Resource->ParamSet;

			GpuParticles::ComputeCommand command;
			command.PipelineStatePtr = pipelineParticleUpdate_;

			command.UniformBufferPtrs[0] = computeConstantsUniformBuffer_;
			command.UniformBufferPtrs[1] = emitter.Resource->ParamBuffer;
			command.UniformBufferPtrs[2] = emitter.Buffer;
			command.SetComputeBuffer(0, particlesComputeBuffer_, false);
			command.SetComputeBuffer(1, trailsComputeBuffer_, false);

			command.SetTexture(2, (emitter.Resource->NoiseTexture) ? emitter.Resource->NoiseTexture : dummyVectorTexture_, Effekseer::Backend::TextureWrapType::Repeat, Effekseer::Backend::TextureSamplingType::Linear);
			command.SetTexture(3, (emitter.Resource->FieldTexture) ? emitter.Resource->FieldTexture : dummyVectorTexture_, Effekseer::Backend::TextureWrapType::Repeat, Effekseer::Backend::TextureSamplingType::Linear);
			command.SetTexture(4, (emitter.Resource->GradientTexture) ? emitter.Resource->GradientTexture : dummyColorTexture_, Effekseer::Backend::TextureWrapType::Clamp, Effekseer::Backend::TextureSamplingType::Linear);

			command.GroupCount = {(int32_t)emitter.Data.ParticleSize / 256, 1, 1};
			command.ThreadCount = {256, 1, 1};
			graphicsDevice_->Dispatch(command);

			if (emitter.Data.TrailSize > 0)
			{
				emitter.Data.TrailPhase = (emitter.Data.TrailPhase + 1) % paramSet.RenderShape.Data;
			}
		}
	}
}

void GpuParticleSystem::RenderFrame(const Context& context)
{
	using namespace Effekseer::GpuParticles;

	auto renderer = renderer_;

	// Update constant buffer
	{
		GpuParticles::RenderConstants cdata{};
		cdata.CoordinateReversed = context.CoordinateReversed;
		cdata.ProjMat = renderer->GetProjectionMatrix();
		cdata.CameraMat = renderer->GetCameraMatrix();

		Effekseer::Matrix44 inv{};
		Effekseer::Matrix44::Inverse(inv, cdata.CameraMat);
		cdata.BillboardMat = {
			float4{inv.Values[0][0], inv.Values[1][0], inv.Values[2][0], 0.0f},
			float4{inv.Values[0][1], inv.Values[1][1], inv.Values[2][1], 0.0f},
			float4{inv.Values[0][2], inv.Values[1][2], inv.Values[2][2], 0.0f}};
		cdata.YAxisFixedMat = {
			float4{inv.Values[0][0], 0.0f, inv.Values[2][0], 0.0f},
			float4{inv.Values[0][1], 1.0f, inv.Values[2][1], 0.0f},
			float4{inv.Values[0][2], 0.0f, inv.Values[2][2], 0.0f}};
		auto normalize = [](Effekseer::Vector3D v)
		{
			Effekseer::Vector3D::Normal(v, v);
			return v;
		};
		cdata.CameraPos = renderer->GetCameraPosition();
		cdata.CameraFront = normalize(renderer->GetCameraFrontDirection());
		cdata.LightDir = normalize(renderer->GetLightDirection());
		cdata.LightColor = renderer->GetLightColor().ToFloat4();
		cdata.LightAmbient = renderer->GetLightAmbientColor().ToFloat4();
		graphicsDevice_->UpdateUniformBuffer(renderConstantsUniformBuffer_, sizeof(GpuParticles::RenderConstants), 0, &cdata);
	}

	auto toSamplingType = [](uint8_t filterType)
	{
		return (static_cast<Effekseer::TextureFilterType>(filterType) == Effekseer::TextureFilterType::Linear) ? Effekseer::Backend::TextureSamplingType::Linear : Effekseer::Backend::TextureSamplingType::Nearest;
	};
	auto toWrapType = [](uint8_t wrapType)
	{
		return (static_cast<Effekseer::TextureWrapType>(wrapType) == Effekseer::TextureWrapType::Repeat) ? Effekseer::Backend::TextureWrapType::Repeat : Effekseer::Backend::TextureWrapType::Clamp;
	};

	for (EmitterID emitterID = 0; emitterID < (EmitterID)emitters_.size(); emitterID++)
	{
		auto& emitter = emitters_[emitterID];
		if (emitter.IsAlive())
		{
			auto effect = emitter.Resource->Effect;
			auto& paramSet = emitter.Resource->ParamSet;

			Effekseer::Backend::DrawParameter drawParams;
			drawParams.PipelineStatePtr = GetOrCreatePipelineState(emitter.Resource->GPUPipelineStateKey);

			drawParams.VertexUniformBufferPtrs[0] = drawParams.PixelUniformBufferPtrs[0] = renderConstantsUniformBuffer_;
			drawParams.VertexUniformBufferPtrs[1] = drawParams.PixelUniformBufferPtrs[1] = emitter.Resource->ParamBuffer;
			drawParams.VertexUniformBufferPtrs[2] = emitter.Buffer;

			drawParams.SetComputeBuffer(0, particlesComputeBuffer_);
			drawParams.SetComputeBuffer(1, trailsComputeBuffer_);

			auto setTexture = [&](int32_t slot, Effekseer::TextureRef texture, GpuParticles::TextureRef defaultTexture)
			{
				drawParams.SetTexture(2 + slot, texture ? texture->GetBackend() : defaultTexture, toWrapType(paramSet.RenderMaterial.TextureWraps[slot]), toSamplingType(paramSet.RenderMaterial.TextureFilters[slot]));
			};

			if (paramSet.RenderMaterial.Material == MaterialType::Unlit)
			{
				auto colorTexture = effect->GetColorImage(paramSet.RenderMaterial.TextureIndexes[0]);
				setTexture(0, colorTexture, dummyColorTexture_);
				setTexture(1, nullptr, dummyNormalTexture_);
			}
			else if (paramSet.RenderMaterial.Material == MaterialType::Lighting)
			{
				auto colorTexture = effect->GetColorImage(paramSet.RenderMaterial.TextureIndexes[0]);
				auto normalTexture = effect->GetNormalImage(paramSet.RenderMaterial.TextureIndexes[1]);
				setTexture(0, colorTexture, dummyColorTexture_);
				setTexture(1, normalTexture, dummyNormalTexture_);
			}
			else
			{
				for (int32_t slot = 0; slot < 4; slot++)
				{
					auto texture = effect->GetColorImage(paramSet.RenderMaterial.TextureIndexes[slot]);
					setTexture(slot, texture, dummyColorTexture_);
				}
			}

			Effekseer::ModelRef model;
			switch (paramSet.RenderShape.Type)
			{
			case RenderShapeT::Sprite:
				model = modelSprite_;
				break;
			case RenderShapeT::Model:
				model = effect->GetModel(paramSet.RenderShape.Data);
				break;
			case RenderShapeT::Trail:
				model = modelTrail_;
				break;
			}

			if (model)
			{
				if (!model->GetIsBufferStoredOnGPU())
				{
					model->StoreBufferToGPU(graphicsDevice_.Get());
				}

				int32_t modelFrameCount = model->GetFrameCount();
				for (int32_t i = 0; i < modelFrameCount; i++)
				{
					drawParams.VertexBufferPtr = model->GetVertexBuffer(i);
					drawParams.VertexStride = sizeof(Effekseer::Model::Vertex);
					drawParams.IndexBufferPtr = model->GetIndexBuffer(i);

					if (emitter.Data.TrailSize > 0)
					{
						drawParams.PrimitiveCount = paramSet.RenderShape.Data * 2;
					}
					else
					{
						drawParams.PrimitiveCount = model->GetFaceCount(i);
					}
					drawParams.InstanceCount = emitter.Data.ParticleSize;

					graphicsDevice_->Draw(drawParams);
				}
			}
		}
	}
}

GpuParticleSystem::EmitterID GpuParticleSystem::NewEmitter(Effekseer::GpuParticles::ResourceRef resource, Effekseer::InstanceGlobal* instanceGlobal)
{
	using namespace Effekseer::GpuParticles;

	std::lock_guard<std::mutex> lock(mutex_);

	if (emitterFreeList_.size() == 0)
	{
		return InvalidID;
	}

	EmitterID emitterID = emitterFreeList_.front();

	auto& paramSet = resource->GetParamSet();
	uint32_t particlesMaxCount = (uint32_t)(round((double)paramSet.Basic.LifeTime[0] * paramSet.Basic.EmitPerFrame));
	particlesMaxCount = std::min(particlesMaxCount, (uint32_t)paramSet.Basic.EmitCount);
	particlesMaxCount = RoundUp(particlesMaxCount, ParticleUnitSize);

	Emitter& emitter = emitters_[emitterID];
	emitter.Resource = resource.DownCast<GpuParticles::Resource>();
	emitter.InstanceGlobal = instanceGlobal;
	emitter.Data = {};
	emitter.Data.Color = 0xFFFFFFFF;
	emitter.Data.Transform = {
		float4{1.0f, 0.0f, 0.0f, 0.0f},
		float4{0.0f, 1.0f, 0.0f, 0.0f},
		float4{0.0f, 0.0f, 1.0f, 0.0f}};
	emitter.SetFlagBits(true, false);

	Block particleBlock = particleAllocator_.Allocate(particlesMaxCount);
	if (particleBlock.Size == 0)
	{
		return InvalidID;
	}
	emitter.Data.ParticleHead = particleBlock.Offset;
	emitter.Data.ParticleSize = particleBlock.Size;

	if (paramSet.RenderShape.Type == RenderShapeT::Trail)
	{
		Block trailBlock = trailAllocator_.Allocate(particlesMaxCount * paramSet.RenderShape.Data);
		if (trailBlock.Size == 0)
		{
			particleAllocator_.Deallocate(particleBlock);
			return InvalidID;
		}
		emitter.Data.TrailHead = trailBlock.Offset;
		emitter.Data.TrailSize = trailBlock.Size;
	}

	newEmitterIds_.push_back(emitterID);
	emitterFreeList_.pop_front();

	return emitterID;
}

void GpuParticleSystem::FreeEmitter(EmitterID emitterID)
{
	assert(emitterID >= 0 && emitterID < emitters_.size());

	Emitter& emitter = emitters_[emitterID];
	emitter.Resource = nullptr;
	emitter.InstanceGlobal = nullptr;
	emitter.Data.FlagBits = 0;

	particleAllocator_.Deallocate({emitter.Data.ParticleHead, emitter.Data.ParticleSize});
	trailAllocator_.Deallocate({emitter.Data.TrailHead, emitter.Data.TrailSize});
	emitterFreeList_.push_front(emitterID);
}

void GpuParticleSystem::StartEmit(EmitterID emitterID)
{
	assert(emitterID >= 0 && emitterID < emitters_.size());
	Emitter& emitter = emitters_[emitterID];
	emitter.SetEmitting(true);
	emitter.Data.TimeCount = 0.0f;
}

void GpuParticleSystem::StopEmit(EmitterID emitterID)
{
	assert(emitterID >= 0 && emitterID < emitters_.size());
	Emitter& emitter = emitters_[emitterID];
	emitter.SetEmitting(false);
	emitter.Data.TimeStopped = emitter.Data.TimeCount;
}

void GpuParticleSystem::SetRandomSeed(EmitterID emitterID, uint32_t seed)
{
	assert(emitterID >= 0 && emitterID < emitters_.size());
	Emitter& emitter = emitters_[emitterID];
	emitter.Data.Seed = seed;
}

void GpuParticleSystem::SetTransform(EmitterID emitterID, const Effekseer::Matrix43& transform)
{
	using namespace Effekseer::GpuParticles;

	assert(emitterID >= 0 && emitterID < emitters_.size());
	Emitter& emitter = emitters_[emitterID];
	emitter.Data.Transform = {
		float4{transform.Value[0][0], transform.Value[1][0], transform.Value[2][0], transform.Value[3][0]},
		float4{transform.Value[0][1], transform.Value[1][1], transform.Value[2][1], transform.Value[3][1]},
		float4{transform.Value[0][2], transform.Value[1][2], transform.Value[2][2], transform.Value[3][2]}};
}

void GpuParticleSystem::SetColor(EmitterID emitterID, Effekseer::Color color)
{
	assert(emitterID >= 0 && emitterID < emitters_.size());
	Emitter& emitter = emitters_[emitterID];
	emitter.Data.Color = *reinterpret_cast<uint32_t*>(&color);
}

void GpuParticleSystem::SetDeltaTime(Effekseer::InstanceGlobal* instanceGlobal, float deltaTime)
{
	for (EmitterID emitterID = 0; emitterID < (EmitterID)emitters_.size(); emitterID++)
	{
		auto& emitter = emitters_[emitterID];
		if (emitter.IsAlive() && emitter.InstanceGlobal == instanceGlobal)
		{
			emitter.Data.DeltaTime = deltaTime;
		}
	}
}

void GpuParticleSystem::KillParticles(Effekseer::InstanceGlobal* instanceGlobal)
{
	std::lock_guard<std::mutex> lock(mutex_);

	for (EmitterID emitterID = 0; emitterID < (EmitterID)emitters_.size(); emitterID++)
	{
		auto& emitter = emitters_[emitterID];
		if (emitter.IsAlive() && emitter.InstanceGlobal == instanceGlobal)
		{
			FreeEmitter(emitterID);
		}
	}
}

int32_t GpuParticleSystem::GetParticleCount(Effekseer::InstanceGlobal* instanceGlobal)
{
	int32_t count = 0;

	for (EmitterID emitterID = 0; emitterID < (EmitterID)emitters_.size(); emitterID++)
	{
		auto& emitter = emitters_[emitterID];
		if (emitter.IsAlive() && emitter.InstanceGlobal == instanceGlobal)
		{
			count += GetEmitterParticleCount(emitter, emitter.Resource->ParamSet);
		}
	}
	return count;
}

int32_t GpuParticleSystem::GetEmitterParticleCount(const Emitter& emitter, const Effekseer::GpuParticles::ParamSet& paramSet)
{
	int32_t maxParticleCount = static_cast<int32_t>(paramSet.Basic.LifeTime[0] * paramSet.Basic.EmitPerFrame);
	float emitDuration = static_cast<float>(paramSet.Basic.EmitCount) / static_cast<float>(paramSet.Basic.EmitPerFrame);
	float timeCount = std::max(0.0f, emitter.Data.TimeCount - paramSet.Basic.EmitOffset);

	if (!emitter.IsEmitting())
	{
		emitDuration = std::min(emitDuration, emitter.Data.TimeStopped - paramSet.Basic.EmitOffset);
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

	for (auto& [cachedKey, cachedPipeline] : pipelineParticleRenders_)
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

	pipParams.IsDepthTestEnabled = key.ZTest > 0;
	pipParams.IsDepthWriteEnabled = key.ZWrite > 0;
	pipParams.ShaderPtr = shaders_.RsParticleRender;
	pipParams.VertexLayoutPtr = vertexLayout_;

	GpuParticles::PipelineStateRef pipeline = graphicsDevice_->CreatePipelineState(pipParams);

	pipelineParticleRenders_.emplace_back(key, pipeline);

	return pipeline;
}

} // namespace EffekseerRenderer

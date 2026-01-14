#pragma once

#include "EffekseerRenderer.Renderer.h"
#include <random>

namespace EffekseerRenderer
{

namespace GpuParticles
{
using float2 = Effekseer::GpuParticles::float2;
using float3 = Effekseer::GpuParticles::float3;
using float4 = Effekseer::GpuParticles::float4;
using float4x3 = Effekseer::GpuParticles::float4x3;
using float4x4 = Effekseer::GpuParticles::float4x4;

using int2 = Effekseer::GpuParticles::int2;
using int3 = Effekseer::GpuParticles::int3;
using int4 = Effekseer::GpuParticles::int4;

using uint = Effekseer::GpuParticles::uint;
using uint2 = Effekseer::GpuParticles::uint2;
using uint3 = Effekseer::GpuParticles::uint3;
using uint4 = Effekseer::GpuParticles::uint4;

#include "Shader/gpu_particles_data.h"

using ComputeBufferRef = Effekseer::Backend::ComputeBufferRef;
using UniformBufferRef = Effekseer::Backend::UniformBufferRef;
using TextureRef = Effekseer::Backend::TextureRef;
using ComputeCommand = Effekseer::Backend::DispatchParameter;
using ShaderRef = Effekseer::Backend::ShaderRef;
using VertexLayoutRef = Effekseer::Backend::VertexLayoutRef;
using PipelineStateRef = Effekseer::Backend::PipelineStateRef;

union PipelineStateKey
{
	uint32_t Data;
	struct
	{
		uint8_t BlendType;
		uint8_t CullingType;
		uint8_t ZTest;
		uint8_t ZWrite;
	};
};

class Resource : public Effekseer::GpuParticles::Resource
{
public:
	Resource() = default;
	~Resource() = default;
	virtual const Effekseer::GpuParticles::ParamSet& GetParamSet() const override
	{
		return ParamSet;
	}

public:
	Effekseer::GpuParticles::ParamSet ParamSet;
	const Effekseer::Effect* Effect = nullptr;
	PipelineStateKey GPUPipelineStateKey;
	UniformBufferRef ParamBuffer;
	TextureRef NoiseTexture;
	TextureRef FieldTexture;
	TextureRef GradientTexture;
	ComputeBufferRef EmitPoints;
	uint32_t EmitPointCount = 0;
};
using ResourceRef = Effekseer::RefPtr<Resource>;

} // namespace GpuParticles

class GpuParticleFactory : public Effekseer::GpuParticleFactory
{
public:
	GpuParticleFactory(Effekseer::Backend::GraphicsDeviceRef graphicsDevice);
	Effekseer::GpuParticles::ResourceRef CreateResource(const Effekseer::GpuParticles::ParamSet& paramSet, const Effekseer::Effect* effect);

protected:
	GpuParticles::ParameterData ToParamData(const Effekseer::GpuParticles::ParamSet& paramSet);
	GpuParticles::PipelineStateKey ToPiplineStateKey(const Effekseer::GpuParticles::ParamSet& paramSet);

private:
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice_;
};

class GpuParticleSystem : public Effekseer::GpuParticleSystem
{
public:
	struct Shaders
	{
		GpuParticles::ShaderRef CsParticleClear;
		GpuParticles::ShaderRef CsParticleSpawn;
		GpuParticles::ShaderRef CsParticleUpdate;
		GpuParticles::ShaderRef RsParticleRender;
	};

	struct Emitter
	{
		GpuParticles::EmitterData Data;
		GpuParticles::UniformBufferRef Buffer;
		Effekseer::RefPtr<GpuParticles::Resource> Resource;
		Effekseer::InstanceGlobal* InstanceGlobal;

		bool IsAlive() const
		{
			return (Data.FlagBits & 1) != 0;
		}
		bool IsEmitting() const
		{
			return (Data.FlagBits & 2) != 0;
		}
		void SetFlagBits(bool alive, bool emission)
		{
			Data.FlagBits = ((uint32_t)alive) | ((uint32_t)emission << 1);
		}
		void SetAlive(bool alive)
		{
			Data.FlagBits = (Data.FlagBits & ~1) | ((uint32_t)alive << 0);
		}
		void SetEmitting(bool emitting)
		{
			Data.FlagBits = (Data.FlagBits & ~2) | ((uint32_t)emitting << 1);
		}
	};
	struct Particle
	{
		GpuParticles::ParticleData Data;
	};
	struct Trail
	{
		GpuParticles::TrailData Data;
	};

	struct Block
	{
		uint32_t Offset;
		uint32_t Size;
	};
	struct BlockAllocator
	{
		std::vector<Block> BufferBlocks;

		void Init(uint32_t bufferSize, uint32_t blockSize);
		Block Allocate(uint32_t size);
		void Deallocate(Block releasingBlock);
	};

public:
	GpuParticleSystem(Renderer* renderer);

	virtual ~GpuParticleSystem();

	virtual bool InitSystem(const Settings& settings) override;

	virtual void SetShaders(const Shaders& shaders);

	virtual void ComputeFrame(const Context& context) override;

	virtual void RenderFrame(const Context& context) override;

	virtual EmitterID NewEmitter(Effekseer::GpuParticles::ResourceRef resource, Effekseer::InstanceGlobal* instanceGlobal) override;

	virtual void StartEmit(EmitterID emitterID) override;

	virtual void StopEmit(EmitterID emitterID) override;

	virtual void SetRandomSeed(EmitterID emitterID, uint32_t seed) override;

	virtual void SetTransform(EmitterID emitterID, const Effekseer::Matrix43& transform) override;

	virtual void SetColor(EmitterID emitterID, Effekseer::Color color) override;

	virtual void SetDeltaTime(Effekseer::InstanceGlobal* instanceGlobal, float deltaTime) override;

	virtual void KillParticles(Effekseer::InstanceGlobal* instanceGlobal) override;

	virtual int32_t GetParticleCount(Effekseer::InstanceGlobal* instanceGlobal) override;

private:
	int32_t GetEmitterParticleCount(const Emitter& emitter, const Effekseer::GpuParticles::ParamSet& paramSet);

	void FreeEmitter(EmitterID emitterID);

	GpuParticles::PipelineStateRef GetOrCreatePipelineState(GpuParticles::PipelineStateKey key);

protected:
	static constexpr uint32_t EmitterUnitSize = 16;
	static constexpr uint32_t ParticleUnitSize = 256;

	Renderer* renderer_ = nullptr;
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice_;

	std::mutex mutex_;
	std::deque<EmitterID> emitterFreeList_;
	std::vector<EmitterID> newEmitterIds_;
	std::vector<Emitter> emitters_;
	BlockAllocator particleAllocator_;
	BlockAllocator trailAllocator_;

	GpuParticles::UniformBufferRef computeConstantsUniformBuffer_;
	GpuParticles::UniformBufferRef renderConstantsUniformBuffer_;

	GpuParticles::ComputeBufferRef particlesComputeBuffer_;
	GpuParticles::ComputeBufferRef trailsComputeBuffer_;

	GpuParticles::PipelineStateRef pipelineParticleClear_;
	GpuParticles::PipelineStateRef pipelineParticleSpawn_;
	GpuParticles::PipelineStateRef pipelineParticleUpdate_;
	Effekseer::CustomVector<std::tuple<GpuParticles::PipelineStateKey, GpuParticles::PipelineStateRef>> pipelineParticleRenders_;

	Shaders shaders_;

	GpuParticles::ComputeBufferRef dummyEmitPoints_;
	GpuParticles::TextureRef dummyVectorTexture_;
	GpuParticles::TextureRef dummyColorTexture_;
	GpuParticles::TextureRef dummyNormalTexture_;

	GpuParticles::VertexLayoutRef vertexLayout_;
	Effekseer::ModelRef modelSprite_;
	Effekseer::ModelRef modelTrail_;
};

} // namespace EffekseerRenderer

#pragma once

#include <random>
#include "EffekseerRenderer.Renderer.h"

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
	virtual const Effekseer::GpuParticles::ParamSet& GetParamSet() const override { return paramSet; }

public:
	Effekseer::GpuParticles::ParamSet paramSet;
	const Effekseer::Effect* effect = nullptr;
	PipelineStateKey piplineStateKey;
	UniformBufferRef paramBuffer;
	TextureRef noiseTexture;
	TextureRef fieldTexture;
	TextureRef gradientTexture;
	ComputeBufferRef emitPoints;
	uint32_t emitPointCount = 0;
};
using ResourceRef = Effekseer::RefPtr<Resource>;

}

class GpuParticleFactory : public Effekseer::GpuParticleFactory
{
public:
	GpuParticleFactory(Effekseer::Backend::GraphicsDeviceRef graphics);
	Effekseer::GpuParticles::ResourceRef CreateResource(const Effekseer::GpuParticles::ParamSet& paramSet, const Effekseer::Effect* effect);

protected:
	GpuParticles::ParameterData ToParamData(const Effekseer::GpuParticles::ParamSet& paramSet);
	GpuParticles::PipelineStateKey ToPiplineStateKey(const Effekseer::GpuParticles::ParamSet& paramSet);

private:
	Effekseer::Backend::GraphicsDeviceRef m_graphics;
};

class GpuParticleSystem : public Effekseer::GpuParticleSystem
{
public:
	struct Shaders
	{
		GpuParticles::ShaderRef csParticleClear;
		GpuParticles::ShaderRef csParticleSpawn;
		GpuParticles::ShaderRef csParticleUpdate;
		GpuParticles::ShaderRef rsParticleRender;
	};

	struct Emitter
	{
		GpuParticles::EmitterData data;
		GpuParticles::UniformBufferRef buffer;
		Effekseer::RefPtr<GpuParticles::Resource> resource;
		Effekseer::InstanceGlobal* instanceGlobal;

		bool IsAlive() const
		{
			return (data.FlagBits & 1) != 0;
		}
		bool IsEmitting() const
		{
			return (data.FlagBits & 2) != 0;
		}
		void SetFlagBits(bool alive, bool emission)
		{
			data.FlagBits = ((uint32_t)alive) | ((uint32_t)emission << 1);
		}
		void SetAlive(bool alive)
		{
			data.FlagBits = (data.FlagBits & ~1) | ((uint32_t)alive << 0);
		}
		void SetEmitting(bool emitting)
		{
			data.FlagBits = (data.FlagBits & ~2) | ((uint32_t)emitting << 1);
		}
	};
	struct Particle
	{
		GpuParticles::ParticleData data;
	};
	struct Trail
	{
		GpuParticles::TrailData data;
	};

	struct Block
	{
		uint32_t offset;
		uint32_t size;
	};
	struct BlockAllocator
	{
		std::vector<Block> bufferBlocks;

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

	Renderer* m_rendererBase;
	std::mutex m_mutex;
	std::deque<EmitterID> m_emitterFreeList;
	std::vector<EmitterID> m_newEmitterIDs;
	std::vector<Emitter> m_emitters;
	BlockAllocator m_particleAllocator;
	BlockAllocator m_trailAllocator;

	GpuParticles::UniformBufferRef m_ubufComputeConstants;
	GpuParticles::UniformBufferRef m_ubufRenderConstants;

	GpuParticles::ComputeBufferRef m_cbufParticles;
	GpuParticles::ComputeBufferRef m_cbufTrails;

	GpuParticles::PipelineStateRef m_pipelineParticleClear;
	GpuParticles::PipelineStateRef m_pipelineParticleSpawn;
	GpuParticles::PipelineStateRef m_pipelineParticleUpdate;
	Effekseer::CustomVector<std::tuple<GpuParticles::PipelineStateKey, GpuParticles::PipelineStateRef>> m_pipelineParticleRenders;

	Shaders m_shaders;

	GpuParticles::ComputeBufferRef m_dummyEmitPoints;
	GpuParticles::TextureRef m_dummyVectorTexture;
	GpuParticles::TextureRef m_dummyColorTexture;
	GpuParticles::TextureRef m_dummyNormalTexture;

	GpuParticles::VertexLayoutRef m_vertexLayout;
	Effekseer::ModelRef m_modelSprite;
	Effekseer::ModelRef m_modelTrail;
};

}

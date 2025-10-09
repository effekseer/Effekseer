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
	GpuParticleFactory(Effekseer::Backend::GraphicsDeviceRef graphics_device);
	Effekseer::GpuParticles::ResourceRef CreateResource(const Effekseer::GpuParticles::ParamSet& paramSet, const Effekseer::Effect* effect);

protected:
	GpuParticles::ParameterData ToParamData(const Effekseer::GpuParticles::ParamSet& paramSet);
	GpuParticles::PipelineStateKey ToPiplineStateKey(const Effekseer::GpuParticles::ParamSet& paramSet);

private:
	Effekseer::Backend::GraphicsDeviceRef graphics_device_;
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

	Renderer* renderer_ = nullptr;
	Effekseer::Backend::GraphicsDeviceRef graphics_device_;

	std::mutex mutex_;
	std::deque<EmitterID> emitter_free_list_;
	std::vector<EmitterID> new_emitter_ids_;
	std::vector<Emitter> emitters_;
	BlockAllocator particle_allocator_;
	BlockAllocator trail_allocator_;

	GpuParticles::UniformBufferRef ubuf_compute_constants_;
	GpuParticles::UniformBufferRef ubuf_render_constants_;

	GpuParticles::ComputeBufferRef cbuf_particles_;
	GpuParticles::ComputeBufferRef cbuf_trails_;

	GpuParticles::PipelineStateRef pipeline_particle_clear_;
	GpuParticles::PipelineStateRef pipeline_particle_spawn_;
	GpuParticles::PipelineStateRef pipeline_particle_update_;
	Effekseer::CustomVector<std::tuple<GpuParticles::PipelineStateKey, GpuParticles::PipelineStateRef>> pipeline_particle_renders_;

	Shaders shaders_;

	GpuParticles::ComputeBufferRef dummy_emit_points_;
	GpuParticles::TextureRef dummy_vector_texture_;
	GpuParticles::TextureRef dummy_color_texture_;
	GpuParticles::TextureRef dummy_normal_texture_;

	GpuParticles::VertexLayoutRef vertex_layout_;
	Effekseer::ModelRef model_sprite_;
	Effekseer::ModelRef model_trail_;
};

}

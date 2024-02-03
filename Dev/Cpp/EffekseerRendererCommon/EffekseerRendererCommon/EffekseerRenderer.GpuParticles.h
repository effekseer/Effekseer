#pragma once

#include <random>
#include "EffekseerRenderer.Renderer.h"

namespace EffekseerRenderer
{

class GpuParticles : public Effekseer::GpuParticles
{
public:
	using ComputeBufferRef = Effekseer::Backend::ComputeBufferRef;
	using UniformBufferRef = Effekseer::Backend::UniformBufferRef;
	using TextureRef = Effekseer::Backend::TextureRef;
	using ComputeCommand = Effekseer::Backend::DispatchParameter;
	using ShaderRef = Effekseer::Backend::ShaderRef;
	using VertexLayoutRef = Effekseer::Backend::VertexLayoutRef;
	using PipelineStateRef = Effekseer::Backend::PipelineStateRef;

	struct Shaders
	{
		ShaderRef csParticleClear;
		ShaderRef csParticleSpawn;
		ShaderRef csParticleUpdate;
		ShaderRef rsParticleRender;
	};

#include "Shader/gpu_particles_data.h"

	class Resource : public Effekseer::GpuParticles::Resource
	{
	public:
		Resource() = default;
		~Resource() = default;
		virtual const ParamSet& GetParamSet() const override { return paramSet; }

	public:
		ParamSet paramSet;
		const Effekseer::Effect* effect = nullptr;
		PipelineStateRef piplineState;
		UniformBufferRef paramBuffer;
		TextureRef noiseTexture;
		TextureRef fieldTexture;
		TextureRef gradientTexture;
		ComputeBufferRef emitPoints;
		uint32_t emitPointCount = 0;
	};

	struct Emitter
	{
		EmitterData data;
		UniformBufferRef buffer;
		Effekseer::RefPtr<Resource> resource;
		ParticleGroupID groupID;

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
		ParticleData data;
	};
	struct Trail
	{
		TrailData data;
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
	GpuParticles(Renderer* renderer);

	virtual ~GpuParticles();

	virtual bool InitSystem(const Settings& settings) override;

	virtual void SetShaders(const Shaders& shaders);

	virtual void ComputeFrame() override;

	virtual void RenderFrame() override;

	virtual ResourceRef CreateResource(const ParamSet& paramSet, const Effekseer::Effect* effect) override;

	virtual EmitterID NewEmitter(ResourceRef resource, ParticleGroupID groupID) override;

	virtual void StartEmit(EmitterID emitterID) override;

	virtual void StopEmit(EmitterID emitterID) override;

	virtual void SetRandomSeed(EmitterID emitterID, uint32_t seed) override;

	virtual void SetDeltaTime(EmitterID emitterID, float deltaTime) override;

	virtual void SetTransform(EmitterID emitterID, const Effekseer::Matrix43& transform) override;

	virtual void SetColor(EmitterID emitterID, Effekseer::Color color) override;

	virtual void KillParticles(ParticleGroupID groupID) override;

	virtual int32_t GetParticleCount(ParticleGroupID groupID) override;

	virtual void ResetDeltaTime() override;

protected:
	PipelineStateRef CreatePiplineState(const ParamSet& paramSet);

private:
	ParameterData ToParamData(const ParamSet& paramSet);

	int32_t GetEmitterParticleCount(const Emitter& emitter, const ParamSet& paramSet);

	void FreeEmitter(EmitterID emitterID);

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

	UniformBufferRef m_ubufConstants;

	ComputeBufferRef m_cbufParticles;
	ComputeBufferRef m_cbufTrails;

	PipelineStateRef m_pipelineParticleClear;
	PipelineStateRef m_pipelineParticleSpawn;
	PipelineStateRef m_pipelineParticleUpdate;

	Shaders m_shaders;

	ComputeBufferRef m_dummyEmitPoints;
	TextureRef m_dummyVectorTexture;
	TextureRef m_dummyColorTexture;
	TextureRef m_dummyNormalTexture;

	VertexLayoutRef m_vertexLayout;
	Effekseer::ModelRef m_modelSprite;
	Effekseer::ModelRef m_modelTrail;
};

}

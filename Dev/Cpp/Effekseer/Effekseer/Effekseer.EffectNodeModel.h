
#ifndef __EFFEKSEER_ParameterNODE_MODEL_H__
#define __EFFEKSEER_ParameterNODE_MODEL_H__

#include "Effekseer.EffectNode.h"
#include "Renderer/Effekseer.ModelRenderer.h"

namespace Effekseer
{

class EffectNodeModel : public EffectNodeImplemented
{
	friend class Manager;
	friend class Effect;
	friend class Instance;

public:
	struct InstanceValues
	{
		// 色
		Color _color;
		Color _original;

		InstanceAllTypeColorState allColorValues;
	};

	ModelRenderer::NodeParameter nodeParam_;

public:
	AlphaBlendType AlphaBlend;
	int32_t ModelIndex;
	BillboardType Billboard;
	CullingType Culling;

	AllTypeColorParameter AllColor;

	ModelReferenceType Mode = ModelReferenceType::File;

	EffectNodeModel(Effect* effect, unsigned char*& pos)
		: EffectNodeImplemented(effect, pos)
	{
	}

	~EffectNodeModel() = default;

	void LoadRendererParameter(unsigned char*& pos, const SettingRef& setting) override;

	void BeginRendering(int32_t count, Manager* manager, const InstanceGlobal* global, void* userData) override;

	void Rendering(const Instance& instance, const Instance* next_instance, int index, Manager* manager, void* userData) override;

	void EndRendering(Manager* manager, void* userData) override;

	void InitializeRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager) override;

	void UpdateRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager) override;

	eEffectNodeType GetType() const override
	{
		return eEffectNodeType::Model;
	}

private:
	ModelRenderer::NodeParameter GetNodeParameter(const Manager* manager, const InstanceGlobal* global);
};

} // namespace Effekseer

#endif // __EFFEKSEER_ParameterNODE_MODEL_H__

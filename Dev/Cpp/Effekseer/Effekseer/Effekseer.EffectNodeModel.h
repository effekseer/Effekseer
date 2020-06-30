
#ifndef __EFFEKSEER_ParameterNODE_MODEL_H__
#define __EFFEKSEER_ParameterNODE_MODEL_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.EffectNode.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
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

		union {
			struct
			{
				Color _color;
			} fixed;

			struct
			{
				Color _color;
			} random;

			struct
			{
				Color start;
				Color end;

			} easing;

			struct
			{
				std::array<float, 4> offset;
			} fcurve_rgba;

		} allColorValues;
	};

#ifdef __EFFEKSEER_BUILD_VERSION16__
	struct FalloffParameter
	{
		enum BlendType
		{
			Add = 0,
			Sub = 1,
			Mul = 2,
		} ColorBlendType;
		Color BeginColor;
		Color EndColor;
		int32_t Pow;
		
		FalloffParameter()
		{
			ColorBlendType = BlendType::Add;
			BeginColor = Color(255, 255, 255, 255);
			EndColor = Color(255, 255, 255, 255);
			Pow = 1;
		}
	};
#endif

public:
	AlphaBlendType AlphaBlend;
	int32_t ModelIndex;

	//! this value is not used
	int32_t NormalTextureIndex;

	BillboardType Billboard;

	//! this value is not used
	bool Lighting;
	CullingType Culling;

	StandardColorParameter AllColor;

#ifdef __EFFEKSEER_BUILD_VERSION16__
	bool EnableFalloff;
	FalloffParameter FalloffParam;
#endif

	EffectNodeModel(Effect* effect, unsigned char*& pos)
		: EffectNodeImplemented(effect, pos)
	{
	}

	~EffectNodeModel()
	{
	}

	void LoadRendererParameter(unsigned char*& pos, Setting* setting) override;

	void BeginRendering(int32_t count, Manager* manager) override;

	void Rendering(const Instance& instance, const Instance* next_instance, Manager* manager) override;

	void EndRendering(Manager* manager) override;

	void InitializeRenderedInstance(Instance& instance, Manager* manager) override;

	void UpdateRenderedInstance(Instance& instance, Manager* manager) override;

	eEffectNodeType GetType() const override
	{
		return EFFECT_NODE_TYPE_MODEL;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_ParameterNODE_MODEL_H__

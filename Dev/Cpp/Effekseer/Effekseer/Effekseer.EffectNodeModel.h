
#ifndef	__EFFEKSEER_ParameterNODE_MODEL_H__
#define	__EFFEKSEER_ParameterNODE_MODEL_H__

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
class EffectNodeModel
	: public EffectNodeImplemented
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

		union 
		{
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
				Color  end;

			} easing;

			struct
			{
				float	offset[4];
			} fcurve_rgba;

		} allColorValues;
	};

public:
	AlphaBlendType		AlphaBlend;
	int32_t			ModelIndex;
	int32_t			NormalTextureIndex;
	BillboardType	Billboard;
	bool			Lighting;
	CullingType	Culling;

	StandardColorParameter	AllColor;

	EffectNodeModel( Effect* effect, unsigned char*& pos )
		: EffectNodeImplemented(effect, pos)
	{
	}

	~EffectNodeModel()
	{
	}

	void LoadRendererParameter(unsigned char*& pos, Setting* setting);

	void BeginRendering(int32_t count, Manager* manager);

	void Rendering(const Instance& instance, const Instance* next_instance, Manager* manager) override;

	void EndRendering(Manager* manager);

	void InitializeRenderedInstance(Instance& instance, Manager* manager);

	void UpdateRenderedInstance(Instance& instance, Manager* manager);

	eEffectNodeType GetType() const { return EFFECT_NODE_TYPE_MODEL; }
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_ParameterNODE_MODEL_H__

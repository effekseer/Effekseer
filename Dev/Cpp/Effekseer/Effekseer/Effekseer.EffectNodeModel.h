
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
		color _color;
		color _original;

		union 
		{
			struct
			{
				color _color;
			} fixed;

			struct
			{
				color _color;
			} random;

			struct
			{
				color start;
				color  end;

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

	void Rendering(const Instance& instance, Manager* manager);

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

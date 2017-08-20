
#ifndef	__EFFEKSEER_ParameterNODE_SPRITE_H__
#define	__EFFEKSEER_ParameterNODE_SPRITE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.EffectNode.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
struct SpriteColorParameter
{
	enum
	{
		Default = 0,
		Fixed = 1,

		Parameter_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{
		
		} def;

		struct
		{
			color ll;
			color lr;
			color ul;
			color ur;
		} fixed;
	};
};

struct SpritePositionParameter
{
	enum
	{
		Default = 0,
		Fixed = 1,

		Parameter_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{
		
		} def;

		struct
		{
			vector2d ll;
			vector2d lr;
			vector2d ul;
			vector2d ur;
		} fixed;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class EffectNodeSprite
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

		color _originalColor;
		
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

		union
		{
	
		} colorValues;

		union
		{
	
		} positionValues;
	};

public:

	AlphaBlendType		AlphaBlend;
	BillboardType	Billboard;

	StandardColorParameter	SpriteAllColor;

	SpriteColorParameter SpriteColor;
	SpritePositionParameter SpritePosition;

	int SpriteTexture;

	EffectNodeSprite( Effect* effect, unsigned char*& pos )
		: EffectNodeImplemented(effect, pos)
	{
	}

	void LoadRendererParameter(unsigned char*& pos, Setting* setting);

	void BeginRendering(int32_t count, Manager* manager);

	void Rendering(const Instance& instance, Manager* manager);

	void EndRendering(Manager* manager);

	void InitializeRenderedInstance(Instance& instance, Manager* manager);

	void UpdateRenderedInstance(Instance& instance, Manager* manager);

	eEffectNodeType GetType() const { return EFFECT_NODE_TYPE_SPRITE; }
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_ParameterNODE_SPRITE_H__

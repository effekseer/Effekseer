
#ifndef __EFFEKSEER_ParameterNODE_SPRITE_H__
#define __EFFEKSEER_ParameterNODE_SPRITE_H__

#include "Effekseer.EffectNode.h"
#include "Renderer/Effekseer.SpriteRenderer.h"

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
			Color ll;
			Color lr;
			Color ul;
			Color ur;
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
			SIMD::Vec2f ll;
			SIMD::Vec2f lr;
			SIMD::Vec2f ul;
			SIMD::Vec2f ur;
		} fixed;
	};
};

class EffectNodeSprite : public EffectNodeImplemented
{
	friend class Manager;
	friend class Effect;
	friend class Instance;

public:
	struct InstanceValues
	{
		Color _color;
		Color _originalColor;
		InstanceAllTypeColorState allColorValues;

		union
		{

		} colorValues;

		union
		{

		} positionValues;
	};

	SpriteRenderer::NodeParameter nodeParam_;

public:
	AlphaBlendType AlphaBlend;
	BillboardType Billboard;

	AllTypeColorParameter SpriteAllColor;

	SpriteColorParameter SpriteColor;
	SpritePositionParameter SpritePosition;

	EffectNodeSprite(Effect* effect, unsigned char*& pos)
		: EffectNodeImplemented(effect, pos)
	{
	}

	void LoadRendererParameter(unsigned char*& pos, const SettingRef& setting) override;

	void BeginRendering(int32_t count, Manager* manager, const InstanceGlobal* global, void* userData) override;

	void Rendering(const Instance& instance, const Instance* next_instance, int index, Manager* manager, void* userData) override;

	void EndRendering(Manager* manager, void* userData) override;

	void InitializeRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager) override;

	void UpdateRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager) override;

	eEffectNodeType GetType() const override
	{
		return eEffectNodeType::Sprite;
	}

private:
	SpriteRenderer::NodeParameter GetNodeParameter(const Manager* manager, const InstanceGlobal* global);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_ParameterNODE_SPRITE_H__

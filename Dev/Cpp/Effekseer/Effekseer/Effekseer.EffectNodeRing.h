
#ifndef __EFFEKSEER_ParameterNODE_RING_H__
#define __EFFEKSEER_ParameterNODE_RING_H__

#include "Effekseer.EffectNode.h"
#include "Renderer/Effekseer.RingRenderer.h"

namespace Effekseer
{

struct RingSingleParameter
{
	enum
	{
		Fixed = 0,
		Random = 1,
		Easing = 2,

		Parameter_DWORD = 0x7fffffff,
	} type;

	union
	{
		float fixed;
		random_float random;
	};

	// TODO : make variant after C++17
	ParameterEasingFloat easing{Version16Alpha9, Version16Alpha9};
};

struct RingLocationParameter
{
	enum
	{
		Fixed = 0,
		PVA = 1,
		Easing = 2,

		Parameter_DWORD = 0x7fffffff,
	} type;

	union
	{
		struct
		{
			vector2d location;
		} fixed;

		struct
		{
			random_vector2d location;
			random_vector2d velocity;
			random_vector2d acceleration;
		} pva;

		easing_vector2d easing;
	};
};

struct RingSingleValues
{
	float current;
	union
	{
		struct
		{

		} fixed;

		struct
		{

		} random;

		InstanceEasing<float> easing;
	};
};

struct RingLocationValues
{
	SIMD::Vec2f current;

	union
	{
		struct
		{

		} fixed;

		struct
		{
			SIMD::Vec2f start;
			SIMD::Vec2f velocity;
			SIMD::Vec2f acceleration;
		} pva;

		struct
		{
			SIMD::Vec2f start;
			SIMD::Vec2f end;
		} easing;
	};
};

struct RingColorValues
{
	Color current;
	Color original;
	InstanceAllTypeColorState allColorValues;
};

enum class RingShapeType : int32_t
{
	Dount,
	Cresient,
};

struct RingShapeParameter
{
	RingShapeType Type = RingShapeType::Dount;
	float StartingFade = 0.0f;
	float EndingFade = 0.0f;
	RingSingleParameter StartingAngle;
	RingSingleParameter EndingAngle;
};

class EffectNodeRing : public EffectNodeImplemented
{
	friend class Manager;
	friend class Effect;
	friend class Instance;

public:
	struct InstanceValues
	{
		RingSingleValues startingAngle;
		RingSingleValues endingAngle;
		RingLocationValues outerLocation;
		RingLocationValues innerLocation;
		RingSingleValues centerRatio;
		RingColorValues outerColor;
		RingColorValues centerColor;
		RingColorValues innerColor;
	};

public:
	AlphaBlendType AlphaBlend;
	BillboardType Billboard;

	int32_t VertexCount;

	RingShapeParameter Shape;

	RingLocationParameter OuterLocation;
	RingLocationParameter InnerLocation;

	RingSingleParameter CenterRatio;

	AllTypeColorParameter OuterColor;
	AllTypeColorParameter CenterColor;
	AllTypeColorParameter InnerColor;

	EffectNodeRing(Effect* effect, unsigned char*& pos)
		: EffectNodeImplemented(effect, pos)
	{
	}

	~EffectNodeRing() = default;

	void LoadRendererParameter(unsigned char*& pos, const SettingRef& setting) override;

	void BeginRendering(int32_t count, Manager* manager, const InstanceGlobal* global, void* userData) override;

	void Rendering(const Instance& instance, const Instance* next_instance, int index, Manager* manager, void* userData) override;

	void EndRendering(Manager* manager, void* userData) override;

	void InitializeRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager) override;

	void UpdateRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager) override;

	eEffectNodeType GetType() const override
	{
		return eEffectNodeType::Ring;
	}

private:
	RingRenderer::NodeParameter nodeParameter;

	void LoadSingleParameter(unsigned char*& pos, RingSingleParameter& param, int version);

	void LoadLocationParameter(unsigned char*& pos, RingLocationParameter& param);

	void InitializeSingleValues(const RingSingleParameter& param, RingSingleValues& values, Manager* manager, IRandObject* rand);

	void InitializeLocationValues(const RingLocationParameter& param, RingLocationValues& values, Manager* manager, IRandObject* rand);

	void UpdateSingleValues(Instance& instance, const RingSingleParameter& param, RingSingleValues& values);

	void UpdateLocationValues(Instance& instance, const RingLocationParameter& param, RingLocationValues& values);
};

} // namespace Effekseer

#endif // __EFFEKSEER_ParameterNODE_RING_H__

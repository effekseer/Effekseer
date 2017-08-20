
#ifndef	__EFFEKSEER_ParameterNODE_RING_H__
#define	__EFFEKSEER_ParameterNODE_RING_H__

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
		easing_float easing;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
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
			random_vector2d	location;
			random_vector2d	velocity;
			random_vector2d	acceleration;
		} pva;
		
		easing_vector2d easing;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RingColorParameter
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
		color fixed;
		random_color random;
		easing_color easing;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RingSingleValues
{
	float	current;
	union
	{
		struct
		{
			
		} fixed;

		struct
		{

		} random;

		struct
		{
			float  start;
			float  end;
		} easing;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RingLocationValues
{
	vector2d	current;
	union
	{
		struct
		{
	
		} fixed;

		struct
		{
			vector2d  start;
			vector2d  velocity;
			vector2d  acceleration;
		} pva;

		struct
		{
			vector2d  start;
			vector2d  end;
		} easing;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RingColorValues
{
	color	current;
	color	original;

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
			color  start;
			color  end;
		} easing;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class EffectNodeRing
	: public EffectNodeImplemented
{
	friend class Manager;
	friend class Effect;
	friend class Instance;

public:

	struct InstanceValues
	{
		RingSingleValues viewingAngle;
		RingLocationValues outerLocation;
		RingLocationValues innerLocation;
		RingSingleValues centerRatio;
		RingColorValues outerColor;
		RingColorValues centerColor;
		RingColorValues innerColor;
	};

public:

	AlphaBlendType		AlphaBlend;
	BillboardType	Billboard;

	int32_t	VertexCount;

	RingSingleParameter	ViewingAngle;

	RingLocationParameter	OuterLocation;
	RingLocationParameter	InnerLocation;
	
	RingSingleParameter	CenterRatio;
	
	RingColorParameter OuterColor;
	RingColorParameter CenterColor;
	RingColorParameter InnerColor;

	int RingTexture;

	EffectNodeRing( Effect* effect, unsigned char*& pos )
		: EffectNodeImplemented(effect, pos)
	{
	}

	~EffectNodeRing()
	{
	}

	void LoadRendererParameter(unsigned char*& pos, Setting* setting);

	void BeginRendering(int32_t count, Manager* manager);

	void Rendering(const Instance& instance, Manager* manager);

	void EndRendering(Manager* manager);

	void InitializeRenderedInstance(Instance& instance, Manager* manager);

	void UpdateRenderedInstance(Instance& instance, Manager* manager);

	eEffectNodeType GetType() const { return EFFECT_NODE_TYPE_RING; }

private:
	void LoadSingleParameter( unsigned char*& pos, RingSingleParameter& param );

	void LoadLocationParameter( unsigned char*& pos, RingLocationParameter& param );
	
	void LoadColorParameter( unsigned char*& pos, RingColorParameter& param );
	
	void InitializeSingleValues(const RingSingleParameter& param, RingSingleValues& values, Manager* manager);

	void InitializeLocationValues(const RingLocationParameter& param, RingLocationValues& values, Manager* manager);
	
	void InitializeColorValues(const RingColorParameter& param, RingColorValues& values, Manager* manager);
	
	void UpdateSingleValues( Instance& instance, const RingSingleParameter& param, RingSingleValues& values );
	
	void UpdateLocationValues( Instance& instance, const RingLocationParameter& param, RingLocationValues& values );
	
	void UpdateColorValues( Instance& instance, const RingColorParameter& param, RingColorValues& values );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_ParameterNODE_RING_H__


#ifndef	__EFFEKSEER_ParameterNODE_RIBBON_H__
#define	__EFFEKSEER_ParameterNODE_RIBBON_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.EffectNode.h"
#include "Renderer/Effekseer.RibbonRenderer.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RibbonAllColorParameter
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
		struct
		{
			color all;
		} fixed;

		struct
		{
			random_color all;
		} random;

		struct
		{
			easing_color all;
		} easing;
	};
};

struct RibbonColorParameter
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
			color l;
			color r;
		} fixed;
	};
};

struct RibbonPositionParameter
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
			float l;
			float r;
		} fixed;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class EffectNodeRibbon
	: public EffectNode
{
public:

	struct InstanceValues
	{
		// êF
		color _color;

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
				color start;
				color  end;

			} easing;

		} allColorValues;

		union
		{
	
		} colorValues;

		union
		{
	
		} positionValues;
	};

	RibbonRenderer::NodeParameter	m_nodeParameter;
	RibbonRenderer::InstanceParameter m_instanceParameter;
public:

	eAlphaBlend		AlphaBlend;

	int				ViewpointDependent;

	RibbonAllColorParameter	RibbonAllColor;

	RibbonColorParameter RibbonColor;
	RibbonPositionParameter RibbonPosition;

	int RibbonTexture;

	EffectNodeRibbon( Effect* effect, unsigned char*& pos )
		: EffectNode( effect, pos )
	{
	}

	~EffectNodeRibbon()
	{
	}

	void LoadRendererParameter( unsigned char*& pos );

	void InitializeRenderer();

	void FinalizeRenderer();

	void BeginRendering( int32_t count );

	void BeginRenderingGroup( InstanceGroup* group );

	void Rendering( const Instance& instance );

	void EndRendering();

	void InitializeRenderedInstance( Instance& instance );

	void UpdateRenderedInstance( Instance& instance );

	eEffectNodeType GetType() const { return EFFECT_NODE_TYPE_RIBBON; }
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_ParameterNODE_RIBBON_H__

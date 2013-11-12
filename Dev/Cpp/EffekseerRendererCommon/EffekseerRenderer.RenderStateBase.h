
#ifndef	__EFFEKSEERRENDERER_RENDERSTATE_BASE_H__
#define	__EFFEKSEERRENDERER_RENDERSTATE_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>
#include <stack>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class RenderStateBase
{
public:
	struct State
	{
		bool								DepthTest				: 1;
		bool								DepthWrite				: 1;
		::Effekseer::eAlphaBlend			AlphaBlend;
		::Effekseer::eCullingType			CullingType;
		::Effekseer::eTextureFilterType		TextureFilterTypes[4];
		::Effekseer::eTextureWrapType		TextureWrapTypes[4];

		State();

		void Reset();

		void CopyTo( State& state );
	};

protected:
	std::stack<State>	m_stateStack;
	State				m_active;
	State				m_next;

public:
	RenderStateBase();
	virtual ~RenderStateBase();

	virtual void Update( bool forced ) = 0;

	State& Push();
	void Pop();
	State& GetActiveState();
};

inline void ColorToFloat4( ::Effekseer::Color color, float fc[4] )
{
	fc[0] = color.R / 255.0f;
	fc[1] = color.G / 255.0f;
	fc[2] = color.B / 255.0f;
	fc[3] = color.A / 255.0f;
}

inline void VectorToFloat4(::Effekseer::Vector3D v, float fc[4])
{
	fc[0] = v.X;
	fc[1] = v.Y;
	fc[2] = v.Z;
	fc[3] = 1.0f;
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_RENDERSTATE_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RenderStateBase.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderStateBase::State::State()
{
	Reset();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void RenderStateBase::State::Reset()
{
	DepthTest = false;
	DepthWrite = false;
	AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
	CullingType = ::Effekseer::CullingType::Double;

	TextureFilterTypes[0] = ::Effekseer::TextureFilterType::Nearest;
	TextureFilterTypes[1] = ::Effekseer::TextureFilterType::Nearest;
	TextureFilterTypes[2] = ::Effekseer::TextureFilterType::Nearest;
	TextureFilterTypes[3] = ::Effekseer::TextureFilterType::Nearest;
	
	TextureWrapTypes[0] = ::Effekseer::TextureWrapType::Clamp;
	TextureWrapTypes[1] = ::Effekseer::TextureWrapType::Clamp;
	TextureWrapTypes[2] = ::Effekseer::TextureWrapType::Clamp;
	TextureWrapTypes[3] = ::Effekseer::TextureWrapType::Clamp;	
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void RenderStateBase::State::CopyTo( State& state )
{
	state.DepthTest = DepthTest;
	state.DepthWrite = DepthWrite;
	state.AlphaBlend = AlphaBlend;
	state.CullingType = CullingType;
	state.TextureFilterTypes[0] = TextureFilterTypes[0];
	state.TextureFilterTypes[1] = TextureFilterTypes[1];
	state.TextureFilterTypes[2] = TextureFilterTypes[2];
	state.TextureFilterTypes[3] = TextureFilterTypes[3];

	state.TextureWrapTypes[0] = TextureWrapTypes[0];
	state.TextureWrapTypes[1] = TextureWrapTypes[1];
	state.TextureWrapTypes[2] = TextureWrapTypes[2];
	state.TextureWrapTypes[3] = TextureWrapTypes[3];
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderStateBase::RenderStateBase()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderStateBase::~RenderStateBase()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderStateBase::State& RenderStateBase::Push()
{
	State newState;
	m_next.CopyTo( newState );
	m_stateStack.push( m_next );
	m_next = newState;
	return m_next;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void RenderStateBase::Pop()
{
	assert( !m_stateStack.empty() );

	State top = m_stateStack.top();
	m_stateStack.pop();

	m_next = top;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderStateBase::State& RenderStateBase::GetActiveState()
{
	return m_next;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

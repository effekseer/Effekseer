
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.ModelRendererBase.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRendererBase::ModelRendererBase()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRendererBase::~ModelRendererBase()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ModelRendererBase::Rendering( const efkModelNodeParam& parameter, const efkModelInstanceParam& instanceParameter, void* userData )
{
	Effekseer::Matrix44 mat;
	for( int32_t r = 0; r < 4; r++ )
	{
		for( int32_t c = 0; c < 3; c++ )
		{
			mat.Values[r][c] = instanceParameter.SRTMatrix43.Value[r][c];
		}
	}

	if( parameter.Magnification != 1.0f )
	{
		Effekseer::Matrix44 mat_scale;
		mat_scale.Values[0][0] = parameter.Magnification;
		mat_scale.Values[1][1] = parameter.Magnification;
		mat_scale.Values[2][2] = parameter.Magnification;

		Effekseer::Matrix44::Mul( mat, mat_scale, mat );
	}

	if( !parameter.IsRightHand )
	{
		Effekseer::Matrix44 mat_scale;
		mat_scale.Values[0][0] = 1.0f;
		mat_scale.Values[1][1] = 1.0f;
		mat_scale.Values[2][2] = -1.0f;

		Effekseer::Matrix44::Mul( mat, mat_scale, mat );
	}

	m_matrixes.push_back( mat );
	m_uv.push_back( instanceParameter.UV );
	m_colors.push_back( instanceParameter.AllColor );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.FCurves.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
FCurve::FCurve( float defaultValue )
	: m_defaultValue	( defaultValue )
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t FCurve::Load( void* data, int32_t version )
{
	int32_t size = 0;
	uint8_t* p = (uint8_t*)data;

	memcpy( &m_start, p, sizeof(int32_t) );
	p += sizeof(int32_t);
	size += sizeof(int32_t);

	memcpy( &m_end, p, sizeof(int32_t) );
	p += sizeof(int32_t);
	size += sizeof(int32_t);

	memcpy( &m_offsetMax, p, sizeof(float) );
	p += sizeof(float);
	size += sizeof(float);

	memcpy( &m_offsetMin, p, sizeof(float) );
	p += sizeof(float);
	size += sizeof(float);

	memcpy( &m_offset, p, sizeof(int32_t) );
	p += sizeof(int32_t);
	size += sizeof(int32_t);

	memcpy( &m_len, p, sizeof(int32_t) );
	p += sizeof(int32_t);
	size += sizeof(int32_t);

	memcpy( &m_freq, p, sizeof(int32_t) );
	p += sizeof(int32_t);
	size += sizeof(int32_t);

	int32_t count = 0;
	memcpy( &count, p, sizeof(int32_t) );
	p += sizeof(int32_t);
	size += sizeof(int32_t);

	for( int32_t i = 0; i < count; i++ )
	{
		float value = 0;

		memcpy( &value, p, sizeof(float) );
		p += sizeof(float);
		size += sizeof(float);

		m_keys.push_back(value);
	}

	return size;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float FCurve::GetValue( int32_t frame )
{
	if( m_keys.size() == 0 ) return m_defaultValue;

	frame -= m_offset;

	if( frame < 0 )
	{
		if( m_start == FC_EDGE_Constant )
		{
			return m_keys[0];
		}
		else if( m_start == FC_EDGE_Loop )
		{
			frame = m_len - (-frame) % m_len;
		}
		else if( m_start == FC_EDGE_LoopInversely )
		{
			frame = (-frame) % m_len;
		}
	}

	if( m_len < frame )
	{
		if( m_end == FC_EDGE_Constant )
		{
			return m_keys[m_keys.size()-1];
		}
		else if( m_end == FC_EDGE_Loop )
		{
			frame = (frame - m_len) % m_len;
		}
		else if( m_end == FC_EDGE_LoopInversely )
		{
			frame = m_len - (frame - m_len) % m_len;
		}
	}
	
	uint32_t ind = frame / m_freq;
	if( frame == m_len )
	{
		return m_keys[m_keys.size()-1];
	}
	else if( ind == m_keys.size() - 1 )
	{
		float subF = (float)(m_len-ind*m_freq);
		float subV = m_keys[ind+1] - m_keys[ind];
		return subV / (float)(subF) * (float)(frame-ind*m_freq) + m_keys[ind];
	}
	else
	{
		float subF = (float)(m_freq);
		float subV = m_keys[ind+1] - m_keys[ind];
		return subV / (float)(subF) * (float)(frame-ind*m_freq) + m_keys[ind];
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float FCurve::GetOffset( const Manager& manager ) const
{
	return (m_offsetMax - m_offsetMin) * ( (float)manager.GetRandFunc()() / (float)manager.GetRandMax() ) + m_offsetMin;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void FCurve::ChangeCoordinate()
{
	m_offsetMax *= -1.0f;
	m_offsetMin *= -1.0f;

	for( size_t i = 0; i < m_keys.size(); i++ )
	{
		m_keys[i] *= -1.0f;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void FCurve::Maginify(float value )
{
	m_offsetMax *= value;
	m_offsetMin *= value;

	for( size_t i = 0; i < m_keys.size(); i++ )
	{
		m_keys[i] *= value;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
FCurveVector2D::FCurveVector2D()
	: X(0)
	, Y(0)
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t FCurveVector2D::Load(void* data, int32_t version)
{
	int32_t size = 0;
	uint8_t* p = (uint8_t*) data;

	int32_t x_size = X.Load(p, version);
	size += x_size;
	p += x_size;

	int32_t y_size = Y.Load(p, version);
	size += y_size;
	p += y_size;

	return size;
}


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
FCurveVector3D::FCurveVector3D()
	: X		( 0 )
	, Y		( 0 )
	, Z		( 0 )
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t FCurveVector3D::Load( void* data, int32_t version )
{
	int32_t size = 0;
	uint8_t* p = (uint8_t*)data;

	int32_t x_size = X.Load( p, version );
	size += x_size;
	p += x_size;

	int32_t y_size = Y.Load( p, version );
	size += y_size;
	p += y_size;

	int32_t z_size = Z.Load( p, version );
	size += z_size;
	p += z_size;

	return size;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
FCurveVectorColor::FCurveVectorColor()
	: R		( 255 )
	, G		( 255 )
	, B		( 255 )
	, A		( 255 )
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t FCurveVectorColor::Load( void* data, int32_t version )
{
	int32_t size = 0;
	uint8_t* p = (uint8_t*)data;

	int32_t x_size = R.Load( p, version );
	size += x_size;
	p += x_size;

	int32_t y_size = G.Load( p, version );
	size += y_size;
	p += y_size;

	int32_t z_size = B.Load( p, version );
	size += z_size;
	p += z_size;

	int32_t w_size = A.Load( p, version );
	size += w_size;
	p += w_size;

	return size;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

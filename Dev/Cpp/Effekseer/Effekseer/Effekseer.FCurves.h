
#ifndef	__EFFEKSEER_FCURVES_H__
#define	__EFFEKSEER_FCURVES_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.InternalStruct.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
	
class FCurve
{
private:
	enum eFCurveEdge
	{
		FC_EDGE_Constant = 0,
		FC_EDGE_Loop = 1,
		FC_EDGE_LoopInversely = 2,
		FC_EDGE_DWORD = 0x7fffffff,
	};

private:
	int32_t				m_offset;
	int32_t				m_len;
	int32_t				m_freq;
	eFCurveEdge			m_start;
	eFCurveEdge			m_end;
	std::vector<float>	m_keys;

	float				m_defaultValue;
	float				m_offsetMax;
	float				m_offsetMin;
public:
	FCurve( float defaultValue );
	int32_t Load( void* data, int32_t version );

	float GetValue( int32_t frame );

	float GetOffset( const Manager& manager ) const;

	void SetDefaultValue( float value ) { m_defaultValue = value; }

	void ChangeCoordinate();

	void Maginify(float value );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class FCurveVector2D
{
public:
	FCurve X;
	FCurve Y;
	
	FCurveVector2D();
	int32_t Load(void* data, int32_t version);
};

class FCurveVector3D
{
public:
	FCurve X;
	FCurve Y;
	FCurve Z;

	FCurveVector3D();
	int32_t Load( void* data, int32_t version );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class FCurveVectorColor
{
public:
	FCurve R;
	FCurve G;
	FCurve B;
	FCurve A;

	FCurveVectorColor();
	int32_t Load( void* data, int32_t version );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_FCURVES_H__

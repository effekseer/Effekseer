

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Vector3D.h"
#include "Effekseer.Matrix43.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer {

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::Indentity()
{
	static const Matrix43 indentity = {{
		{1.0f, 0.0f, 0.0f}, 
		{0.0f, 1.0f, 0.0f}, 
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 0.0f}
	}};
	memcpy( Value, indentity.Value, sizeof(indentity) );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::Scaling( float x, float y, float z )
{
	memset( Value, 0, sizeof(float) * 12 );
	Value[0][0] = x;
	Value[1][1] = y;
	Value[2][2] = z;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::RotationX( float angle )
{
	float c, s;
	::Effekseer::SinCos( angle, s, c );

	Value[0][0] = 1.0f;
	Value[0][1] = 0.0f;
	Value[0][2] = 0.0f;

	Value[1][0] = 0.0f;
	Value[1][1] = c;
	Value[1][2] = s;

	Value[2][0] = 0.0f;
	Value[2][1] = -s;
	Value[2][2] = c;

	Value[3][0] = 0.0f;
	Value[3][1] = 0.0f;
	Value[3][2] = 0.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::RotationY( float angle )
{
	float c, s;
	::Effekseer::SinCos( angle, s, c );

	Value[0][0] = c;
	Value[0][1] = 0.0f;
	Value[0][2] = -s;

	Value[1][0] = 0.0f;
	Value[1][1] = 1.0f;
	Value[1][2] = 0.0f;

	Value[2][0] = s;
	Value[2][1] = 0.0f;
	Value[2][2] = c;

	Value[3][0] = 0.0f;
	Value[3][1] = 0.0f;
	Value[3][2] = 0.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::RotationZ( float angle )
{
	float c, s;
	::Effekseer::SinCos( angle, s, c );

	Value[0][0] = c;
	Value[0][1] = s;
	Value[0][2] = 0.0f;

	Value[1][0] = -s;
	Value[1][1] = c;
	Value[1][2] = 0.0f;

	Value[2][0] = 0.0f;
	Value[2][1] = 0.0f;
	Value[2][2] = 1;

	Value[3][0] = 0.0f;
	Value[3][1] = 0.0f;
	Value[3][2] = 0.0f;
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::RotationXYZ( float rx, float ry, float rz )
{
	float cx, sx, cy, sy, cz, sz;
	
	if( rx != 0.0f )	::Effekseer::SinCos( rx, sx, cx );
	else				sx = 0.0f, cx = 1.0f;
	if( ry != 0.0f )	::Effekseer::SinCos( ry, sy, cy );
	else				sy = 0.0f, cy = 1.0f;
	if( rz != 0.0f )	::Effekseer::SinCos( rz, sz, cz );
	else				sz = 0.0f, cz = 1.0f;

	Value[0][0] = cy * cz;
	Value[0][1] = cy * sz;
	Value[0][2] = -sy;

	Value[1][0] = sx * sy * -sz + cx * -sz;
	Value[1][1] = sx * sy *  sz + cx *  cz;
	Value[1][2] = sx * cy;

	Value[2][0] = cx * sy * cz + sx * sz;
	Value[2][1] = cx * sy * sz - sx * cz;
	Value[2][2] = cx * cy;

	Value[3][0] = 0.0f;
	Value[3][1] = 0.0f;
	Value[3][2] = 0.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::RotationZXY( float rz, float rx, float ry )
{
	float cx, sx, cy, sy, cz, sz;

	if( rx != 0.0f )	::Effekseer::SinCos( rx, sx, cx );
	else				sx = 0.0f, cx = 1.0f;
	if( ry != 0.0f )	::Effekseer::SinCos( ry, sy, cy );
	else				sy = 0.0f, cy = 1.0f;
	if( rz != 0.0f )	::Effekseer::SinCos( rz, sz, cz );
	else				sz = 0.0f, cz = 1.0f;
	
	Value[0][0] = cz * cy + sz * sx * sy;
	Value[0][1] = sz * cx;
	Value[0][2] = cz * -sy + sz * sx * cy;

	Value[1][0] = -sz * cy + cz * sx * sy;
	Value[1][1] = cz * cx;
	Value[1][2] = -sz * -sy + cz * sx * cy;

	Value[2][0] = cx * sy;
	Value[2][1] = -sx;
	Value[2][2] = cx * cy;
	
	Value[3][0] = 0.0f;
	Value[3][1] = 0.0f;
	Value[3][2] = 0.0f;
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::RotationAxis( const Vector3D& axis, float angle )
{
	const float c = cosf( angle );
	const float s = sinf( angle );
	const float cc = 1.0f - c;

	Value[0][0] = cc * (axis.X * axis.X) + c;
	Value[0][1] = cc * (axis.X * axis.Y) + (axis.Z * s);
	Value[0][2] = cc * (axis.Z * axis.X) - (axis.Y * s);

	Value[1][0] = cc * (axis.X * axis.Y) - (axis.Z * s);
	Value[1][1] = cc * (axis.Y * axis.Y) + c;
	Value[1][2] = cc * (axis.Y * axis.Z) + (axis.X * s);

	Value[2][0] = cc * (axis.Z * axis.X) + (axis.Y * s);
	Value[2][1] = cc * (axis.Y * axis.Z) - (axis.X * s);
	Value[2][2] = cc * (axis.Z * axis.Z) + c;

	Value[3][0] = 0.0f;
	Value[3][1] = 0.0f;
	Value[3][2] = 0.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::RotationAxis( const Vector3D& axis, float s, float c )
{
	const float cc = 1.0f - c;

	Value[0][0] = cc * (axis.X * axis.X) + c;
	Value[0][1] = cc * (axis.X * axis.Y) + (axis.Z * s);
	Value[0][2] = cc * (axis.Z * axis.X) - (axis.Y * s);

	Value[1][0] = cc * (axis.X * axis.Y) - (axis.Z * s);
	Value[1][1] = cc * (axis.Y * axis.Y) + c;
	Value[1][2] = cc * (axis.Y * axis.Z) + (axis.X * s);

	Value[2][0] = cc * (axis.Z * axis.X) + (axis.Y * s);
	Value[2][1] = cc * (axis.Y * axis.Z) - (axis.X * s);
	Value[2][2] = cc * (axis.Z * axis.Z) + c;

	Value[3][0] = 0.0f;
	Value[3][1] = 0.0f;
	Value[3][2] = 0.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::Translation( float x, float y, float z )
{
	Indentity();
	Value[3][0] = x;
	Value[3][1] = y;
	Value[3][2] = z;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::GetSRT( Vector3D& s, Matrix43& r, Vector3D& t ) const
{
	t.X = Value[3][0];
	t.Y = Value[3][1];
	t.Z = Value[3][2];
	
	float sc[3];
	for( int m = 0; m < 3; m++ )
	{
		sc[m] = sqrt( Value[m][0] * Value[m][0] + Value[m][1] * Value[m][1] + Value[m][2] * Value[m][2] );
	}
	
	s.X = sc[0];
	s.Y = sc[1];
	s.Z = sc[2];
	
	for( int m = 0; m < 3; m++ )
	{
		for( int n = 0; n < 3; n++ )
		{
			r.Value[m][n] = Value[m][n] / sc[m];
		}
	}
	r.Value[3][0] = 0.0f;
	r.Value[3][1] = 0.0f;
	r.Value[3][2] = 0.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::GetScale( Vector3D& s ) const
{
	float sc[3];
	for( int m = 0; m < 3; m++ )
	{
		sc[m] = sqrt( Value[m][0] * Value[m][0] + Value[m][1] * Value[m][1] + Value[m][2] * Value[m][2] );
	}
	
	s.X = sc[0];
	s.Y = sc[1];
	s.Z = sc[2];
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::GetRotation( Matrix43& r ) const
{
	float sc[3];
	for( int m = 0; m < 3; m++ )
	{
		sc[m] = sqrt( Value[m][0] * Value[m][0] + Value[m][1] * Value[m][1] + Value[m][2] * Value[m][2] );
	}

	for( int m = 0; m < 3; m++ )
	{
		for( int n = 0; n < 3; n++ )
		{
			r.Value[m][n] = Value[m][n] / sc[m];
		}
	}
	r.Value[3][0] = 0.0f;
	r.Value[3][1] = 0.0f;
	r.Value[3][2] = 0.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::GetTranslation( Vector3D& t ) const
{
	t.X = Value[3][0];
	t.Y = Value[3][1];
	t.Z = Value[3][2];
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::SetSRT( const Vector3D& s, const Matrix43& r, const Vector3D& t )
{
	Matrix43 mats;
	mats.Scaling( s.X, s.Y, s.Z );

	Multiple( *this, mats, r );

	Value[3][0] = t.X;
	Value[3][1] = t.Y;
	Value[3][2] = t.Z;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Matrix43::Multiple( Matrix43& out, const Matrix43& in1, const Matrix43& in2 )
{
#if 1
	Matrix43 temp1, temp2;
	// 共通の場合は一時変数にコピー
	const Matrix43& s1 = (&out == &in1) ? (temp1 = in1) : in1;
	const Matrix43& s2 = (&out == &in2) ? (temp2 = in2) : in2;
	
	out.Value[0][0] = s1.Value[0][0] * s2.Value[0][0] + s1.Value[0][1] * s2.Value[1][0] + s1.Value[0][2] * s2.Value[2][0];
	out.Value[0][1] = s1.Value[0][0] * s2.Value[0][1] + s1.Value[0][1] * s2.Value[1][1] + s1.Value[0][2] * s2.Value[2][1];
	out.Value[0][2] = s1.Value[0][0] * s2.Value[0][2] + s1.Value[0][1] * s2.Value[1][2] + s1.Value[0][2] * s2.Value[2][2];
									   					 				 				   				   
	out.Value[1][0] = s1.Value[1][0] * s2.Value[0][0] + s1.Value[1][1] * s2.Value[1][0] + s1.Value[1][2] * s2.Value[2][0];
	out.Value[1][1] = s1.Value[1][0] * s2.Value[0][1] + s1.Value[1][1] * s2.Value[1][1] + s1.Value[1][2] * s2.Value[2][1];
	out.Value[1][2] = s1.Value[1][0] * s2.Value[0][2] + s1.Value[1][1] * s2.Value[1][2] + s1.Value[1][2] * s2.Value[2][2];
									   					 				 				   				   
	out.Value[2][0] = s1.Value[2][0] * s2.Value[0][0] + s1.Value[2][1] * s2.Value[1][0] + s1.Value[2][2] * s2.Value[2][0];
	out.Value[2][1] = s1.Value[2][0] * s2.Value[0][1] + s1.Value[2][1] * s2.Value[1][1] + s1.Value[2][2] * s2.Value[2][1];
	out.Value[2][2] = s1.Value[2][0] * s2.Value[0][2] + s1.Value[2][1] * s2.Value[1][2] + s1.Value[2][2] * s2.Value[2][2];
									   					 				 				   				   
	out.Value[3][0] = s1.Value[3][0] * s2.Value[0][0] + s1.Value[3][1] * s2.Value[1][0] + s1.Value[3][2] * s2.Value[2][0] + s2.Value[3][0];
	out.Value[3][1] = s1.Value[3][0] * s2.Value[0][1] + s1.Value[3][1] * s2.Value[1][1] + s1.Value[3][2] * s2.Value[2][1] + s2.Value[3][1];
	out.Value[3][2] = s1.Value[3][0] * s2.Value[0][2] + s1.Value[3][1] * s2.Value[1][2] + s1.Value[3][2] * s2.Value[2][2] + s2.Value[3][2];
#else
	Matrix43 temp;

	for( int i = 0; i < 4; i++ )
	{
		for( int j = 0; j < 3; j++ )
		{
			float v = 0.0f;
			for( int k = 0; k < 3; k++ )
			{
				v += in1.Value[i][k] * in2.Value[k][j];
			}
			temp.Value[i][j] = v;
		}
	}

	for( int i = 0; i < 3; i++ )
	{
		temp.Value[3][i] += in2.Value[3][i];
	}

	out = temp;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
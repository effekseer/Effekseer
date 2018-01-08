
#ifndef	__EFFEKSEER_INTERNAL_STRUCT_H__
#define	__EFFEKSEER_INTERNAL_STRUCT_H__

/**
	@file
	@brief	内部計算用構造体
	@note
	union等に使用するための構造体。<BR>

*/

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Manager.h"
#include "Effekseer.Vector2D.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Color.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

template <typename T>
T ReadData( unsigned char*& pos )
{
	T result = *(T*)pos;
	pos += sizeof(T);
	return result;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct random_float
{
	float	max;
	float	min;

	void reset()
	{
		memset( this, 0 , sizeof(random_float) );
	};

	float getValue(IRandObject& g) const
	{
		float r;
		r = g.GetRand(min, max);
		return r;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct random_int
{
	int	max;
	int	min;

	void reset()
	{
		memset( this, 0 , sizeof(random_int) );
	};

	float getValue(IRandObject& g) const
	{
		float r;
		r = (int32_t)g.GetRand(min, max);
		return r;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct vector2d
{
	float	x;
	float	y;

	void reset()
	{
		assert( sizeof(vector2d) == sizeof(float) * 2 );
		memset( this, 0, sizeof(vector2d) );
	};

	void setValueToArg( Vector2D& v ) const
	{
		v.X = x;
		v.Y = y;
	}

	vector2d operator + ( const vector2d& o ) const
	{
		vector2d ret;
		ret.x = x + o.x;
		ret.y = y + o.y;
		return ret;
	}

	vector2d operator * (const float& o) const
	{
		vector2d ret;
		ret.x = x * o;
		ret.y = y * o;
		return ret;
	}

	vector2d& operator += ( const vector2d& o )
	{
		x += o.x;
		y += o.y;
		return *this;
	}

	vector2d& operator *= ( const float& o )
	{
		x *= o;
		y *= o;
		return *this;
	}
};

struct rectf
{
	float	x;
	float	y;
	float	w;
	float	h;
	void reset()
	{
		assert( sizeof(rectf) == sizeof(float) * 4 );
		memset( this, 0, sizeof(rectf) );
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct random_vector2d
{
	vector2d	max;
	vector2d	min;

	void reset()
	{
		memset( this, 0 , sizeof(random_vector2d) );
	};

	vector2d getValue(IRandObject& g) const
	{
		vector2d r;
		r.x = g.GetRand(min.x, max.x);
		r.y = g.GetRand(min.y, max.y);
		return r;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct easing_float_without_random
{
	float easingA;
	float easingB;
	float easingC;

	void setValueToArg( float& o, const float start_, const float end_, float t ) const
	{
		float df = end_ - start_;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o = start_ + d * df;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct easing_float
{
	random_float	start;
	random_float	end;
	float easingA;
	float easingB;
	float easingC;

	void setValueToArg( float& o, const float start_, const float end_, float t ) const
	{
		float df = end_ - start_;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o = start_ + d * df;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct easing_vector2d
{
	random_vector2d	start;
	random_vector2d	end;
	float easingA;
	float easingB;
	float easingC;

	void setValueToArg( vector2d& o, const vector2d& start_, const vector2d& end_, float t ) const
	{
		float d_x = end_.x - start_.x;
		float d_y = end_.y - start_.y;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o.x = start_.x + d * d_x;
		o.y = start_.y + d * d_y;
	}

	void setValueToArg( Vector2D& o, const vector2d& start_, const vector2d& end_, float t ) const
	{
		float d_x = end_.x - start_.x;
		float d_y = end_.y - start_.y;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o.X = start_.x + d * d_x;
		o.Y = start_.y + d * d_y;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct vector3d
{
	float	x;
	float	y;
	float	z;

	void reset()
	{
		assert( sizeof(vector3d) == sizeof(float) * 3 );
		memset( this, 0, sizeof(vector3d) );
	};

	void normalize()
	{
		float len = sqrtf(x * x + y * y + z * z);
		if (len > 0.0001f)
		{
			x /= len;
			y /= len;
			z /= len;
		}
		else
		{
			x = 1.0;
			y = 0.0;
			z = 0.0;
		}
	}

	void setValueToArg( Vector3D& v ) const
	{
		v.X = x;
		v.Y = y;
		v.Z = z;
	}

	vector3d operator + ( const vector3d& o ) const
	{
		vector3d ret;
		ret.x = x + o.x;
		ret.y = y + o.y;
		ret.z = z + o.z;
		return ret;
	}

	vector3d operator - ( const vector3d& o ) const
	{
		vector3d ret;
		ret.x = x - o.x;
		ret.y = y - o.y;
		ret.z = z - o.z;
		return ret;
	}

	vector3d operator * ( const float& o ) const
	{
		vector3d ret;
		ret.x = x * o;
		ret.y = y * o;
		ret.z = z * o;
		return ret;
	}

	vector3d& operator += ( const vector3d& o )
	{
		x += o.x;
		y += o.y;
		z += o.z;
		return *this;
	}

	vector3d& operator -= ( const vector3d& o )
	{
		x -= o.x;
		y -= o.y;
		z -= o.z;
		return *this;
	}

	vector3d& operator *= ( const float& o )
	{
		x *= o;
		y *= o;
		z *= o;
		return *this;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct random_vector3d
{
	vector3d	max;
	vector3d	min;

	void reset()
	{
		memset( this, 0 , sizeof(random_vector3d) );
	};

	vector3d getValue(IRandObject& g) const
	{
		vector3d r;
		r.x = g.GetRand(min.x, max.x);
		r.y = g.GetRand(min.y, max.y);
		r.z = g.GetRand(min.z, max.z);
		return r;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct easing_vector3d
{
	random_vector3d	start;
	random_vector3d	end;
	float easingA;
	float easingB;
	float easingC;

	void setValueToArg( vector3d& o, const vector3d& start_, const vector3d& end_, float t ) const
	{
		float d_x = end_.x - start_.x;
		float d_y = end_.y - start_.y;
		float d_z = end_.z - start_.z;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o.x = start_.x + d * d_x;
		o.y = start_.y + d * d_y;
		o.z = start_.z + d * d_z;
	}

	void setValueToArg( Vector3D& o, const vector3d& start_, const vector3d& end_, float t ) const
	{
		float d_x = end_.x - start_.x;
		float d_y = end_.y - start_.y;
		float d_z = end_.z - start_.z;
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o.X = start_.x + d * d_x;
		o.Y = start_.y + d * d_y;
		o.Z = start_.z + d * d_z;
	}
};

inline Color HSVToRGB(Color hsv) {
	int H = hsv.R, S = hsv.G, V = hsv.B;
	int Hi, R=0, G=0, B=0, p, q, t;
	float f, s;

	if (H >= 252) H = 252;
	Hi = (H / 42);
	f = H / 42.0f - Hi;
	Hi = Hi % 6;
	s = S / 255.0f;
	p = (int)((V * (1 - s)));
	q = (int)((V * (1 - f * s)));
	t = (int)((V * (1 - (1 - f) * s)));

	switch (Hi) {
	case 0: R = V; G = t; B = p; break;
	case 1: R = q; G = V; B = p; break;
	case 2: R = p; G = V; B = t; break;
	case 3: R = p; G = q; B = V; break;
	case 4: R = t; G = p; B = V; break;
	case 5: R = V; G = p; B = q; break;
	}
	Color result;
	result.R = R;
	result.G = G;
	result.B = B;
	result.A = hsv.A;
	return result;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct random_color
{
	ColorMode mode;
	Color	max;
	Color	min;

	void reset()
	{
		assert( sizeof(random_color) == 12 );
		mode = COLOR_MODE_RGBA;
		max = {255, 255, 255, 255};
		min = {255, 255, 255, 255};
	};

	Color getValue(IRandObject& g) const
	{
		Color r = getDirectValue( g );
		if( mode == COLOR_MODE_HSVA )
		{
			r = HSVToRGB( r );
		}
		return r;
	}
	
	Color getDirectValue(IRandObject& g) const
	{
		Color r;
		r.R = (uint8_t) (g.GetRand(min.R, max.R));
		r.G = (uint8_t) (g.GetRand(min.G, max.G));
		r.B = (uint8_t) (g.GetRand(min.B, max.B));
		r.A = (uint8_t) (g.GetRand(min.A, max.A));
		return r;
	}

	void load( int version, unsigned char*& pos )
	{
		if( version >= 4 )
		{
			mode = (ColorMode)ReadData<uint8_t>( pos );
			pos++;	// reserved
		}
		else
		{
			mode = COLOR_MODE_RGBA;
		}
		max = ReadData<Color>( pos );
		min = ReadData<Color>( pos );
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct easing_color
{
	random_color	start;
	random_color	end;
	float easingA;
	float easingB;
	float easingC;

	void setValueToArg( Color& o, const Color& start_, const Color& end_, float t ) const
	{
		assert( start.mode == end.mode );
		float d = easingA * t * t * t + easingB * t * t + easingC * t;
		o = Color::Lerp(start_, end_, d);
		if( start.mode == COLOR_MODE_HSVA )
		{
			o = HSVToRGB( o );
		}
	}

	Color getStartValue(IRandObject& g) const
	{
		return start.getDirectValue( g );
	}
	
	Color getEndValue(IRandObject& g) const
	{
		return end.getDirectValue( g);
	}

	void load( int version, unsigned char*& pos )
	{
		start.load( version, pos );
		end.load( version, pos );
		easingA = ReadData<float>( pos );
		easingB = ReadData<float>( pos );
		easingC = ReadData<float>( pos );
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_INTERNAL_STRUCT_H__
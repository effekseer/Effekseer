#pragma once

#include "../Effekseer.Base.h"

#include "../Effekseer.InternalStruct.h"
#include "../Parameter/DynamicParameter.h"

#include <flatbuffers/flatbuffers.h>

using namespace Effekseer::Data;
#include "CommonStructures_generated.h"
#include "EffectStructures_generated.h"

namespace Effekseer
{

class FBConverter
{
public:
	static BindType Convert(FB::Effect::BindType value)
	{
		return static_cast<BindType>(value);
	}

	static RefMinMax Convert(const FB::RefMinMax* value)
	{
		RefMinMax ret;
		if (value != nullptr)
		{
			ret.Max = value->max();
			ret.Min = value->min();
		}
		return ret;
	}

	static random_int Convert(const FB::IntRange* value)
	{
		random_int ret;
		ret.reset();
		if (value != nullptr)
		{
			ret.min = value->min();
			ret.max = value->max();
		}
		return ret;
	}

	static random_float Convert(const FB::FloatRange* value)
	{
		random_float ret;
		ret.reset();
		if (value != nullptr)
		{
			ret.min = value->min();
			ret.max = value->max();
		}
		return ret;
	}

	static void Convert(random_float& dst, const FB::FloatRange* value)
	{
		dst.reset();
		if (value != nullptr)
		{
			dst.min = value->min();
			dst.max = value->max();
		}
	}

	static void Convert(RefMinMax& dst, const FB::RefMinMax* value)
	{
		if (value != nullptr)
		{
			dst.Max = value->max();
			dst.Min = value->min();
		}
	}

	static void Convert(vector3d& dst, const FB::Vec3F* value)
	{
		if (value != nullptr)
		{
			dst.x = value->x();
			dst.y = value->y();
			dst.z = value->z();
		}
	}

	static void Convert(Vector3D& dst, const FB::Vec3F* value)
	{
		if (value != nullptr)
		{
			dst.X = value->x();
			dst.Y = value->y();
			dst.Z = value->z();
		}
	}

	static void Convert(random_vector3d& dst, const FB::Vec3FRange* value)
	{
		dst.reset();
		if (value != nullptr)
		{
			Convert(dst.min, &(value->min()));
			Convert(dst.max, &(value->max()));
		}
	}
};

} // namespace Effekseer
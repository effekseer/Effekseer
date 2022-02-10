#pragma once

#include <Effekseer.h>
#include <stdint.h>

namespace Effekseer::Tool
{

struct Matrix44F
{
	Matrix44F()
	{
		Value.Indentity();
	}

#if !defined(SWIG)
	//! TODO : Refactor
	Effekseer::Matrix44 Value;

	Matrix44F(const Effekseer::Matrix44& v)
		: Value(v)
	{
	}

	operator Effekseer::Matrix44() const
	{
		return Value;
	}
#endif
};

} // namespace Effekseer::Tool

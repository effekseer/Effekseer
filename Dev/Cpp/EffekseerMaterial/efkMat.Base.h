
#pragma once

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace EffekseerMaterial
{
enum class ConnectResultType
{
	OK,
	Type,
	Multi,
	Loop,
	SamePin,
	SameDirection,
	SameNode,
};

enum class WarningType
{
	None,
	WrongInputType,
	WrongProperty
};

enum class ValueType
{
	Float1 = 0,
	Float2 = 1,
	Float3 = 2,
	Float4 = 3,
	FloatN,
	Bool,
	Texture,
	String,
	Function,
	Unknown,
};

enum class PinDirectionType
{
	Input,
	Output,
};

enum class NodeType
{
	Constant1,
	Constant2,

	Param1,
	Param4,

	Abs,
	Sine,
	Add,
	Subtract,
	Multiply,
	Divide,
	FMod,
	OneMinus,
	ComponentMask,
	Append,
	UV,
	Panner,

	ConstantTexture,
	ParamTexture,
	SampleTexture,

	Time,

	Fresnel, // not implemented

	Comment,
	Function, // Unimplemented
	Output,

	// VectrToRadialValue
};

enum class DefaultType
{
	Value,
	UV,
	Time,
};

class PinParameter;
class NodeParameter;

class Material;
class Pin;
class Node;
class Link;
class Library;

inline bool IsFloatValueType(ValueType vt)
{
	return vt == ValueType::Float1 || vt == ValueType::Float2 || vt == ValueType::Float3 || vt == ValueType::Float4 ||
		   vt == ValueType::FloatN;
}

inline int GetElementCount(ValueType vt)
{
	if (vt == ValueType::Float1)
		return 1;
	if (vt == ValueType::Float2)
		return 2;
	if (vt == ValueType::Float3)
		return 3;
	if (vt == ValueType::Float4)
		return 4;
	return 1;
};

} // namespace EffekseerMaterial

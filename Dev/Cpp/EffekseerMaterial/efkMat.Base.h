
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

const int UserTextureSlotMax = 6;

enum class TextureValueType
{
	Color,
	Value,
};

enum class TextureSamplerType
{
	Repeat,
	Wrap,
	Unknown,
};

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
	WrongProperty,
	DifferentSampler,
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
	Enum,
	Int,
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
	Constant3,
	Constant4,

	Parameter1,
	Parameter4,

	Abs,
	Sine,
	Arctangent2,
	Add,
	Subtract,
	Multiply,
	Divide,
	FMod,

	Ceil,
	Floor,
	Frac,
	Max,
	Min,
	Power,
	SquareRoot,
	Clamp,
	DotProduct,
	CrossProduct,
	Normalize,	//! 1500
	LinearInterpolate,

	OneMinus,
	ComponentMask,
	AppendVector,
	TextureCoordinate,
	Panner,

	TextureObject,
	TextureObjectParameter,
	SampleTexture,

	Time,
	CameraPositionWS, //! 1500

	VertexNormalWS,
	PixelNormalWS,

	WorldPosition, //! 1500
	VertexColor,
	ObjectScale, //! 1500
	

	CustomData1,
	CustomData2,

	Fresnel, // not implemented

	Comment,
	Function, // Unimplemented
	Output,

// VectrToRadialValue

//! not supported on UE4
#ifdef _DEBUG
	VertexTangentWS,
#endif

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

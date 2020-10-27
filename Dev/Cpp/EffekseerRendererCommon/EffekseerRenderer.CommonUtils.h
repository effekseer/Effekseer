
#ifndef __EFFEKSEERRENDERER_COMMON_UTILS_H__
#define __EFFEKSEERRENDERER_COMMON_UTILS_H__

#include <Effekseer.Internal.h>
#include <Effekseer.h>
#include <Effekseer/Material/Effekseer.CompiledMaterial.h>
#include <algorithm>
#include <array>
#include <assert.h>
#include <functional>
#include <math.h>
#include <string.h>

namespace EffekseerRenderer
{

using VertexFloat3 = ::Effekseer::Vector3D;
using VertexColor = ::Effekseer::Color;

struct DynamicVertex
{
	VertexFloat3 Pos;
	VertexColor Col;
	//! packed vector
	VertexColor Normal;
	//! packed vector
	VertexColor Tangent;

	union {
		//! UV1 (for template)
		float UV[2];
		float UV1[2];
	};

	float UV2[2];

#ifdef __EFFEKSEER_BUILD_VERSION16__
	float AlphaUV[2];

	float FlipbookIndexAndNextRate;

	float AlphaThreshold;
#endif

	void SetColor(const VertexColor& color)
	{
		Col = color;
	}
};

struct DynamicVertexWithCustomData
{
	DynamicVertex V;
	std::array<float, 4> CustomData1;
	std::array<float, 4> CustomData2;
};

struct SimpleVertex
{
	VertexFloat3 Pos;
	uint8_t Col[4];

	union {
		float UV[2];
		//! dummy for template
		float UV2[2];
	};

#ifdef __EFFEKSEER_BUILD_VERSION16__
	float AlphaUV[2];

	float FlipbookIndexAndNextRate;

	float AlphaThreshold;
#endif

	void SetColor(const ::Effekseer::Color& color)
	{
		Col[0] = color.R;
		Col[1] = color.G;
		Col[2] = color.B;
		Col[3] = color.A;
	}
};

struct SimpleVertexDX9
{
	VertexFloat3 Pos;
	uint8_t Col[4];

	union {
		float UV[2];
		//! dummy for template
		float UV2[2];
	};

#ifdef __EFFEKSEER_BUILD_VERSION16__
	float AlphaUV[2];

	float FlipbookIndexAndNextRate;

	float AlphaThreshold;
#endif

	void SetColor(const ::Effekseer::Color& color)
	{
		Col[0] = color.B;
		Col[1] = color.G;
		Col[2] = color.R;
		Col[3] = color.A;
	}
};

struct VertexDistortion
{
	VertexFloat3 Pos;
	uint8_t Col[4];

	union {
		float UV[2];
		//! dummy for template
		float UV2[2];
	};

	VertexFloat3 Binormal;
	VertexFloat3 Tangent;

#ifdef __EFFEKSEER_BUILD_VERSION16__
	float AlphaUV[2];

	float FlipbookIndexAndNextRate;

	float AlphaThreshold;
#endif

	void SetColor(const ::Effekseer::Color& color)
	{
		Col[0] = color.R;
		Col[1] = color.G;
		Col[2] = color.B;
		Col[3] = color.A;
	}
};

struct VertexDistortionDX9
{
	VertexFloat3 Pos;
	uint8_t Col[4];

	union {
		float UV[2];
		//! dummy for template
		float UV2[2];
	};

	VertexFloat3 Binormal;
	VertexFloat3 Tangent;

#ifdef __EFFEKSEER_BUILD_VERSION16__
	float AlphaUV[2];

	float FlipbookIndexAndNextRate;

	float AlphaThreshold;
#endif

	void SetColor(const ::Effekseer::Color& color)
	{
		Col[0] = color.B;
		Col[1] = color.G;
		Col[2] = color.R;
		Col[3] = color.A;
	}
};

static int32_t GetMaximumVertexSizeInAllTypes()
{
	size_t size = sizeof(SimpleVertex);
	size = (std::max)(size, sizeof(VertexDistortion));
	size = (std::max)(size, sizeof(DynamicVertexWithCustomData));
	return static_cast<int32_t>(size);
};

/**
	@brief	a view class to access an array with a stride
*/
template <typename T>
struct StrideView
{
	int32_t stride_;
	uint8_t* pointer_;
	uint8_t* pointerOrigin_;

#ifndef NDEBUG
	int32_t offset_;
	int32_t elementCount_;
#endif

	StrideView(void* pointer, int32_t stride, int32_t elementCount)
		: stride_(stride)
		, pointer_(reinterpret_cast<uint8_t*>(pointer))
		, pointerOrigin_(reinterpret_cast<uint8_t*>(pointer))
#ifndef NDEBUG
		, offset_(0)
		, elementCount_(elementCount)
#endif
	{
	}

	T& operator[](int i) const
	{
#ifndef NDEBUG
		assert(i >= 0);
		assert(i + offset_ < elementCount_);
#endif
		return *reinterpret_cast<T*>((pointer_ + stride_ * i));
	}

	StrideView& operator+=(const int& rhs)
	{
#ifndef NDEBUG
		offset_ += rhs;
#endif
		pointer_ += stride_ * rhs;
		return *this;
	}

	void Reset()
	{
#ifndef NDEBUG
		offset_ = 0;
#endif
		pointer_ = pointerOrigin_;
	}
};

/**
	@brief	a view class to access an array with a stride
*/
template <>
struct StrideView<SimpleVertex>
{
	static const int32_t stride_ = sizeof(SimpleVertex);
	uint8_t* pointer_;
	uint8_t* pointerOrigin_;

#ifndef NDEBUG
	int32_t offset_;
	int32_t elementCount_;
#endif

	StrideView(void* pointer, int32_t stride, int32_t elementCount)
		: pointer_(reinterpret_cast<uint8_t*>(pointer))
		, pointerOrigin_(reinterpret_cast<uint8_t*>(pointer))
#ifndef NDEBUG
		, offset_(0)
		, elementCount_(elementCount)
#endif
	{
		assert(stride_ == stride);
	}

	SimpleVertex& operator[](int i) const
	{
#ifndef NDEBUG
		assert(i >= 0);
		assert(i + offset_ < elementCount_);
#endif
		return *reinterpret_cast<SimpleVertex*>((pointer_ + stride_ * i));
	}

	StrideView& operator+=(const int& rhs)
	{
#ifndef NDEBUG
		offset_ += rhs;
#endif
		pointer_ += stride_ * rhs;
		return *this;
	}

	void Reset()
	{
#ifndef NDEBUG
		offset_ = 0;
#endif
		pointer_ = pointerOrigin_;
	}
};

/**
	@brief	a view class to access an array with a stride
*/
template <>
struct StrideView<SimpleVertexDX9>
{
	static const int32_t stride_ = sizeof(SimpleVertexDX9);
	uint8_t* pointer_;
	uint8_t* pointerOrigin_;

#ifndef NDEBUG
	int32_t offset_;
	int32_t elementCount_;
#endif

	StrideView(void* pointer, int32_t stride, int32_t elementCount)
		: pointer_(reinterpret_cast<uint8_t*>(pointer))
		, pointerOrigin_(reinterpret_cast<uint8_t*>(pointer))
#ifndef NDEBUG
		, offset_(0)
		, elementCount_(elementCount)
#endif
	{
		assert(stride_ == stride);
	}

	SimpleVertexDX9& operator[](int i) const
	{
#ifndef NDEBUG
		assert(i >= 0);
		assert(i + offset_ < elementCount_);
#endif
		return *reinterpret_cast<SimpleVertexDX9*>((pointer_ + stride_ * i));
	}

	StrideView& operator+=(const int& rhs)
	{
#ifndef NDEBUG
		offset_ += rhs;
#endif
		pointer_ += stride_ * rhs;
		return *this;
	}

	void Reset()
	{
#ifndef NDEBUG
		offset_ = 0;
#endif
		pointer_ = pointerOrigin_;
	}
};

void CalcBillboard(::Effekseer::BillboardType billboardType,
				   Effekseer::Mat43f& dst,
				   ::Effekseer::Vec3f& s,
				   ::Effekseer::Vec3f& R,
				   ::Effekseer::Vec3f& F,
				   const ::Effekseer::Mat43f& src,
				   const ::Effekseer::Vec3f& frontDirection);

/**
	@brief Spline generator
	@note
	Reference https://qiita.com/edo_m18/items/f2f0c6bf9032b0ec12d4
*/
class SplineGenerator
{
	Effekseer::CustomAlignedVector<Effekseer::Vec3f> a;
	Effekseer::CustomAlignedVector<Effekseer::Vec3f> b;
	Effekseer::CustomAlignedVector<Effekseer::Vec3f> c;
	Effekseer::CustomAlignedVector<Effekseer::Vec3f> d;
	Effekseer::CustomAlignedVector<Effekseer::Vec3f> w;
	std::vector<bool> isSame;

public:
	void AddVertex(const Effekseer::Vec3f& v);

	void Calculate();

	void Reset();

	Effekseer::Vec3f GetValue(float t) const;
};

void ApplyDepthParameters(::Effekseer::Mat43f& mat,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand);

void ApplyDepthParameters(::Effekseer::Mat43f& mat,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::Vec3f& scaleValues,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand);

void ApplyDepthParameters(::Effekseer::Mat43f& mat,
						  ::Effekseer::Vec3f& translationValues,
						  ::Effekseer::Vec3f& scaleValues,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand);

void ApplyDepthParameters(::Effekseer::Mat44f& mat,
						  const ::Effekseer::Vec3f& cameraFront,
						  const ::Effekseer::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand);

template <typename Vertex>
inline void TransformStandardVertexes(Vertex& vertexes, int32_t count, const ::Effekseer::Mat43f& mat)
{
	using namespace Effekseer;

	SIMD4f m0 = mat.X;
	SIMD4f m1 = mat.Y;
	SIMD4f m2 = mat.Z;
	SIMD4f m3 = SIMD4f::SetZero();
	SIMD4f::Transpose(m0, m1, m2, m3);

	for (int i = 0; i < count; i++)
	{
		SIMD4f iPos = SIMD4f::Load3(&vertexes[i].Pos);

		SIMD4f oPos = SIMD4f::MulAddLane<0>(m3, m0, iPos);
		oPos = SIMD4f::MulAddLane<1>(oPos, m1, iPos);
		oPos = SIMD4f::MulAddLane<2>(oPos, m2, iPos);

		SIMD4f::Store3(&vertexes[i].Pos, oPos);
	}
}

template <typename VertexDistortion>
inline void TransformDistortionVertexes(VertexDistortion& vertexes, int32_t count, const ::Effekseer::Mat43f& mat)
{
	using namespace Effekseer;

	SIMD4f m0 = mat.X;
	SIMD4f m1 = mat.Y;
	SIMD4f m2 = mat.Z;
	SIMD4f m3 = SIMD4f::SetZero();
	SIMD4f::Transpose(m0, m1, m2, m3);

	for (int i = 0; i < count; i++)
	{
		SIMD4f iPos = SIMD4f::Load3(&vertexes[i].Pos);
		SIMD4f iTangent = SIMD4f::Load3(&vertexes[i].Tangent);
		SIMD4f iBinormal = SIMD4f::Load3(&vertexes[i].Binormal);

		SIMD4f oPos = SIMD4f::MulAddLane<0>(m3, m0, iPos);
		oPos = SIMD4f::MulAddLane<1>(oPos, m1, iPos);
		oPos = SIMD4f::MulAddLane<2>(oPos, m2, iPos);

		SIMD4f oTangent = SIMD4f::MulLane<0>(m0, iTangent);
		oTangent = SIMD4f::MulAddLane<1>(oTangent, m1, iTangent);
		oTangent = SIMD4f::MulAddLane<2>(oTangent, m2, iTangent);

		SIMD4f oBinormal = SIMD4f::MulLane<0>(m0, iBinormal);
		oBinormal = SIMD4f::MulAddLane<1>(oBinormal, m1, iBinormal);
		oBinormal = SIMD4f::MulAddLane<2>(oBinormal, m2, iBinormal);

		SIMD4f::Store3(&vertexes[i].Pos, oPos);
		SIMD4f::Store3(&vertexes[i].Tangent, oTangent);
		SIMD4f::Store3(&vertexes[i].Binormal, oBinormal);
	}
}

inline void TransformVertexes(StrideView<VertexDistortion>& v, int32_t count, const ::Effekseer::Mat43f& mat)
{
	TransformDistortionVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<VertexDistortionDX9>& v, int32_t count, const ::Effekseer::Mat43f& mat)
{
	TransformDistortionVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<SimpleVertex>& v, int32_t count, const ::Effekseer::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<SimpleVertexDX9>& v, int32_t count, const ::Effekseer::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<DynamicVertex>& v, int32_t count, const ::Effekseer::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void SwapRGBAToBGRA(Effekseer::Color& color)
{
	auto temp = color;
	color.B = temp.R;
	color.R = temp.B;
}

inline Effekseer::Color PackVector3DF(const Effekseer::Vec3f& v)
{
	Effekseer::Color ret;
	ret.R = static_cast<uint8_t>(Effekseer::Clamp(((v.GetX() + 1.0f) / 2.0f + 0.5f / 255.0f) * 255.0f, 255, 0));
	ret.G = static_cast<uint8_t>(Effekseer::Clamp(((v.GetY() + 1.0f) / 2.0f + 0.5f / 255.0f) * 255.0f, 255, 0));
	ret.B = static_cast<uint8_t>(Effekseer::Clamp(((v.GetZ() + 1.0f) / 2.0f + 0.5f / 255.0f) * 255.0f, 255, 0));
	ret.A = 255;
	return ret;
}

inline Effekseer::Vec3f SafeNormalize(const Effekseer::Vec3f& v)
{
	auto lengthSq = v.GetSquaredLength();
	auto e = 0.0001f;
	if (lengthSq < e * e)
	{
		return v;
	}

	return v * Effekseer::Rsqrt(lengthSq);
}

struct MaterialShaderParameterGenerator
{
	int32_t VertexSize = 0;
	int32_t VertexShaderUniformBufferSize = 0;
	int32_t PixelShaderUniformBufferSize = 0;

	int32_t VertexCameraMatrixOffset = -1;
	int32_t VertexProjectionMatrixOffset = -1;
	int32_t VertexInversedFlagOffset = -1;
	int32_t VertexPredefinedOffset = -1;
	int32_t VertexCameraPositionOffset = -1;
	int32_t VertexUserUniformOffset = -1;

	int32_t PixelInversedFlagOffset = -1;
	int32_t PixelPredefinedOffset = -1;
	int32_t PixelCameraPositionOffset = -1;
	int32_t PixelLightDirectionOffset = -1;
	int32_t PixelLightColorOffset = -1;
	int32_t PixelLightAmbientColorOffset = -1;
	int32_t PixelCameraMatrixOffset = -1;
	int32_t PixelUserUniformOffset = -1;

	int32_t VertexModelMatrixOffset = -1;
	int32_t VertexModelUVOffset = -1;
	int32_t VertexModelColorOffset = -1;

	int32_t VertexModelCustomData1Offset = -1;
	int32_t VertexModelCustomData2Offset = -1;

	MaterialShaderParameterGenerator(const ::Effekseer::Material& material, bool isModel, int32_t stage, int32_t instanceCount)
	{
		if (isModel)
		{
			VertexSize = sizeof(::Effekseer::Model::Vertex);
		}
		else if (material.GetIsSimpleVertex())
		{
			VertexSize = sizeof(EffekseerRenderer::SimpleVertex);
		}
		else
		{
			VertexSize = sizeof(EffekseerRenderer::DynamicVertex) +
						 sizeof(float) * (material.GetCustomData1Count() + material.GetCustomData2Count());
		}

		if (isModel)
		{
			int32_t vsOffset = 0;
			VertexProjectionMatrixOffset = vsOffset;
			vsOffset += sizeof(Effekseer::Mat44f);

			VertexModelMatrixOffset = vsOffset;
			vsOffset += sizeof(Effekseer::Mat44f) * instanceCount;

			VertexModelUVOffset = vsOffset;
			vsOffset += sizeof(float) * 4 * instanceCount;

			VertexModelColorOffset = vsOffset;
			vsOffset += sizeof(float) * 4 * instanceCount;

			VertexInversedFlagOffset = vsOffset;
			vsOffset += sizeof(float) * 4;

			VertexPredefinedOffset = vsOffset;
			vsOffset += sizeof(float) * 4;

			VertexCameraPositionOffset = vsOffset;
			vsOffset += sizeof(float) * 4;

			if (material.GetCustomData1Count() > 0)
			{
				VertexModelCustomData1Offset = vsOffset;
				vsOffset += sizeof(float) * 4 * instanceCount;
			}

			if (material.GetCustomData2Count() > 0)
			{
				VertexModelCustomData2Offset = vsOffset;
				vsOffset += sizeof(float) * 4 * instanceCount;
			}

			VertexUserUniformOffset = vsOffset;
			vsOffset += sizeof(float) * 4 * material.GetUniformCount();

			VertexShaderUniformBufferSize = vsOffset;
		}
		else
		{
			int32_t vsOffset = 0;
			VertexCameraMatrixOffset = vsOffset;
			vsOffset += sizeof(Effekseer::Mat44f);

			VertexProjectionMatrixOffset = vsOffset;
			vsOffset += sizeof(Effekseer::Mat44f);

			VertexInversedFlagOffset = vsOffset;
			vsOffset += sizeof(float) * 4;

			VertexPredefinedOffset = vsOffset;
			vsOffset += sizeof(float) * 4;

			VertexCameraPositionOffset = vsOffset;
			vsOffset += sizeof(float) * 4;

			VertexUserUniformOffset = vsOffset;
			vsOffset += sizeof(float) * 4 * material.GetUniformCount();

			VertexShaderUniformBufferSize = vsOffset;
		}

		int32_t psOffset = 0;

		PixelInversedFlagOffset = psOffset;
		psOffset += sizeof(float) * 4;

		PixelPredefinedOffset = psOffset;
		psOffset += sizeof(float) * 4;

		PixelCameraPositionOffset = psOffset;
		psOffset += sizeof(float) * 4;

		if (material.GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
		{
			PixelLightDirectionOffset = psOffset;
			psOffset += sizeof(float) * 4;

			PixelLightColorOffset = psOffset;
			psOffset += sizeof(float) * 4;

			PixelLightAmbientColorOffset = psOffset;
			psOffset += sizeof(float) * 4;
		}

		if (material.GetHasRefraction() && stage == 1)
		{
			PixelCameraMatrixOffset = psOffset;
			psOffset += sizeof(Effekseer::Mat44f);
		}

		PixelUserUniformOffset = psOffset;
		psOffset += sizeof(float) * 4 * material.GetUniformCount();

		PixelShaderUniformBufferSize = psOffset;
	}
};

} // namespace EffekseerRenderer
#endif // __EFFEKSEERRENDERER_COMMON_UTILS_H__
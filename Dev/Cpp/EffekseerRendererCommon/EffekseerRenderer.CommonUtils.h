
#ifndef __EFFEKSEERRENDERER_COMMON_UTILS_H__
#define __EFFEKSEERRENDERER_COMMON_UTILS_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.h"
#include <Effekseer.h>
#include <Effekseer/Material/Effekseer.CompiledMaterial.h>
#include <Effekseer/Model/SplineGenerator.h>
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

	void SetAlphaUV(float value, int index)
	{
	}

	void SetUVDistortionUV(float value, int index)
	{
	}

	void SetBlendUV(float value, int index)
	{
	}

	void SetBlendAlphaUV(float value, int index)
	{
	}

	void SetBlendUVDistortionUV(float value, int index)
	{
	}

	void SetFlipbookIndexAndNextRate(float value)
	{
	}

	void SetAlphaThreshold(float value)
	{
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
	}

	void SetPackedNormal(const VertexColor& normal)
	{
		Normal = normal;
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
		Tangent = tangent;
	}

	void SetUV2(float u, float v)
	{
		UV2[0] = u;
		UV2[1] = v;
	}
};

struct DynamicVertexWithCustomData
{
	DynamicVertex V;
	std::array<float, 4> CustomData1;
	std::array<float, 4> CustomData2;
};

struct LightingVertex
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

	void SetAlphaUV(float value, int index)
	{
	}
	void SetUVDistortionUV(float value, int index)
	{
	}
	void SetBlendUV(float value, int index)
	{
	}
	void SetBlendAlphaUV(float value, int index)
	{
	}
	void SetBlendUVDistortionUV(float value, int index)
	{
	}
	void SetFlipbookIndexAndNextRate(float value)
	{
	}
	void SetAlphaThreshold(float value)
	{
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
	}

	void SetPackedNormal(const VertexColor& normal)
	{
		Normal = normal;
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
		Tangent = tangent;
	}

	void SetUV2(float u, float v)
	{
		UV2[0] = u;
		UV2[1] = v;
	}
};

struct SimpleVertex
{
	VertexFloat3 Pos;
	VertexColor Col;

	union {
		float UV[2];
		//! dummy for template
		float UV2[2];
	};

	void SetAlphaUV(float value, int index)
	{
	}
	void SetUVDistortionUV(float value, int index)
	{
	}
	void SetBlendUV(float value, int index)
	{
	}
	void SetBlendAlphaUV(float value, int index)
	{
	}
	void SetBlendUVDistortionUV(float value, int index)
	{
	}
	void SetFlipbookIndexAndNextRate(float value)
	{
	}
	void SetAlphaThreshold(float value)
	{
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
	}

	void SetPackedNormal(const VertexColor& normal)
	{
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
	}

	void SetUV2(float u, float v)
	{
	}
};

struct VertexDistortion
{
	VertexFloat3 Pos;
	VertexColor Col;

	union {
		float UV[2];
		//! dummy for template
		float UV2[2];
	};

	VertexFloat3 Binormal;
	VertexFloat3 Tangent;

	void SetAlphaUV(float value, int index)
	{
	}
	void SetUVDistortionUV(float value, int index)
	{
	}
	void SetBlendUV(float value, int index)
	{
	}
	void SetBlendAlphaUV(float value, int index)
	{
	}
	void SetBlendUVDistortionUV(float value, int index)
	{
	}
	void SetFlipbookIndexAndNextRate(float value)
	{
	}
	void SetAlphaThreshold(float value)
	{
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
		Tangent = tangent;
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
		Binormal = binormal;
	}

	void SetPackedNormal(const VertexColor& normal)
	{
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
	}

	void SetUV2(float u, float v)
	{
	}
};

struct AdvancedLightingVertex
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

	float AlphaUV[2];
	float UVDistortionUV[2];
	float BlendUV[2];
	float BlendAlphaUV[2];
	float BlendUVDistortionUV[2];
	float FlipbookIndexAndNextRate;
	float AlphaThreshold;

	void SetAlphaUV(float value, int index)
	{
		AlphaUV[index] = value;
	}
	void SetUVDistortionUV(float value, int index)
	{
		UVDistortionUV[index] = value;
	}
	void SetBlendUV(float value, int index)
	{
		BlendUV[index] = value;
	}
	void SetBlendAlphaUV(float value, int index)
	{
		BlendAlphaUV[index] = value;
	}
	void SetBlendUVDistortionUV(float value, int index)
	{
		BlendUVDistortionUV[index] = value;
	}
	void SetFlipbookIndexAndNextRate(float value)
	{
		FlipbookIndexAndNextRate = value;
	}
	void SetAlphaThreshold(float value)
	{
		AlphaThreshold = value;
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
	}

	void SetPackedNormal(const VertexColor& normal)
	{
		Normal = normal;
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
		Tangent = tangent;
	}

	void SetUV2(float u, float v)
	{
		UV2[0] = u;
		UV2[1] = v;
	}
};

struct AdvancedSimpleVertex
{
	VertexFloat3 Pos;
	VertexColor Col;

	union {
		float UV[2];
		//! dummy for template
		float UV2[2];
	};

	float AlphaUV[2];
	float UVDistortionUV[2];
	float BlendUV[2];
	float BlendAlphaUV[2];
	float BlendUVDistortionUV[2];
	float FlipbookIndexAndNextRate;
	float AlphaThreshold;

	void SetAlphaUV(float value, int index)
	{
		AlphaUV[index] = value;
	}
	void SetUVDistortionUV(float value, int index)
	{
		UVDistortionUV[index] = value;
	}
	void SetBlendUV(float value, int index)
	{
		BlendUV[index] = value;
	}
	void SetBlendAlphaUV(float value, int index)
	{
		BlendAlphaUV[index] = value;
	}
	void SetBlendUVDistortionUV(float value, int index)
	{
		BlendUVDistortionUV[index] = value;
	}
	void SetFlipbookIndexAndNextRate(float value)
	{
		FlipbookIndexAndNextRate = value;
	}
	void SetAlphaThreshold(float value)
	{
		AlphaThreshold = value;
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
	}

	void SetPackedNormal(const VertexColor& normal)
	{
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
	}

	void SetUV2(float u, float v)
	{
	}
};

struct AdvancedVertexDistortion
{
	VertexFloat3 Pos;
	VertexColor Col;

	union {
		float UV[2];
		//! dummy for template
		float UV2[2];
	};

	VertexFloat3 Binormal;
	VertexFloat3 Tangent;

	float AlphaUV[2];
	float UVDistortionUV[2];
	float BlendUV[2];
	float BlendAlphaUV[2];
	float BlendUVDistortionUV[2];
	float FlipbookIndexAndNextRate;
	float AlphaThreshold;

	void SetAlphaUV(float value, int index)
	{
		AlphaUV[index] = value;
	}
	void SetUVDistortionUV(float value, int index)
	{
		UVDistortionUV[index] = value;
	}
	void SetBlendUV(float value, int index)
	{
		BlendUV[index] = value;
	}
	void SetBlendAlphaUV(float value, int index)
	{
		BlendAlphaUV[index] = value;
	}
	void SetBlendUVDistortionUV(float value, int index)
	{
		BlendUVDistortionUV[index] = value;
	}
	void SetFlipbookIndexAndNextRate(float value)
	{
		FlipbookIndexAndNextRate = value;
	}
	void SetAlphaThreshold(float value)
	{
		AlphaThreshold = value;
	}

	void SetColor(const VertexColor& color, bool flipRGB)
	{
		Col = color;

		if (flipRGB)
		{
			std::swap(Col.R, Col.B);
		}
	}

	void SetTangent(const VertexFloat3& tangent)
	{
		Tangent = tangent;
	}

	void SetBinormal(const VertexFloat3& binormal)
	{
		Binormal = binormal;
	}

	void SetPackedNormal(const VertexColor& normal)
	{
	}

	void SetPackedTangent(const VertexColor& tangent)
	{
	}

	void SetUV2(float u, float v)
	{
	}
};

static int32_t GetMaximumVertexSizeInAllTypes()
{
	size_t size = sizeof(DynamicVertexWithCustomData);
	size = (std::max)(size, sizeof(VertexDistortion));
	size = (std::max)(size, sizeof(SimpleVertex));
	size = (std::max)(size, sizeof(LightingVertex));
	size = (std::max)(size, sizeof(AdvancedVertexDistortion));
	size = (std::max)(size, sizeof(AdvancedSimpleVertex));
	size = (std::max)(size, sizeof(AdvancedLightingVertex));

	return static_cast<int32_t>(size);
};

template <typename T>
inline bool IsDistortionVertex()
{
	return false;
}

template <>
inline bool IsDistortionVertex<VertexDistortion>()
{
	return true;
}

template <>
inline bool IsDistortionVertex<AdvancedVertexDistortion>()
{
	return true;
}

template <typename T>
inline bool IsDynamicVertex()
{
	return false;
}

template <>
inline bool IsDynamicVertex<DynamicVertex>()
{
	return true;
}

template <typename T>
inline bool IsLightingVertex()
{
	return false;
}

template <>
inline bool IsLightingVertex<LightingVertex>()
{
	return true;
}

template <>
inline bool IsLightingVertex<AdvancedLightingVertex>()
{
	return true;
}

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

void CalcBillboard(::Effekseer::BillboardType billboardType,
				   Effekseer::SIMD::Mat43f& dst,
				   ::Effekseer::SIMD::Vec3f& s,
				   ::Effekseer::SIMD::Vec3f& R,
				   ::Effekseer::SIMD::Vec3f& F,
				   const ::Effekseer::SIMD::Mat43f& src,
				   const ::Effekseer::SIMD::Vec3f& frontDirection);

void ApplyDepthParameters(::Effekseer::SIMD::Mat43f& mat,
						  const ::Effekseer::SIMD::Vec3f& cameraFront,
						  const ::Effekseer::SIMD::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand);

void ApplyDepthParameters(::Effekseer::SIMD::Mat43f& mat,
						  const ::Effekseer::SIMD::Vec3f& cameraFront,
						  const ::Effekseer::SIMD::Vec3f& cameraPos,
						  ::Effekseer::SIMD::Vec3f& scaleValues,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand);

void ApplyDepthParameters(::Effekseer::SIMD::Mat43f& mat,
						  ::Effekseer::SIMD::Vec3f& translationValues,
						  ::Effekseer::SIMD::Vec3f& scaleValues,
						  const ::Effekseer::SIMD::Vec3f& cameraFront,
						  const ::Effekseer::SIMD::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand);

void ApplyDepthParameters(::Effekseer::SIMD::Mat44f& mat,
						  const ::Effekseer::SIMD::Vec3f& cameraFront,
						  const ::Effekseer::SIMD::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand);

void ApplyViewOffset(::Effekseer::SIMD::Mat43f& mat,
					 const ::Effekseer::SIMD::Mat44f& camera,
					 float distance);

void ApplyViewOffset(::Effekseer::SIMD::Mat44f& mat,
					 const ::Effekseer::SIMD::Mat44f& camera,
					 float distance);

template <typename Vertex>
inline void TransformStandardVertexes(Vertex& vertexes, int32_t count, const ::Effekseer::SIMD::Mat43f& mat)
{
	using namespace Effekseer::SIMD;

	Float4 m0 = mat.X;
	Float4 m1 = mat.Y;
	Float4 m2 = mat.Z;
	Float4 m3 = Float4::SetZero();
	Float4::Transpose(m0, m1, m2, m3);

	for (int i = 0; i < count; i++)
	{
		Float4 iPos = Float4::Load3(&vertexes[i].Pos);

		Float4 oPos = Float4::MulAddLane<0>(m3, m0, iPos);
		oPos = Float4::MulAddLane<1>(oPos, m1, iPos);
		oPos = Float4::MulAddLane<2>(oPos, m2, iPos);

		Float4::Store3(&vertexes[i].Pos, oPos);
	}
}

template <typename VertexDistortion>
inline void TransformDistortionVertexes(VertexDistortion& vertexes, int32_t count, const ::Effekseer::SIMD::Mat43f& mat)
{
	using namespace Effekseer::SIMD;

	Float4 m0 = mat.X;
	Float4 m1 = mat.Y;
	Float4 m2 = mat.Z;
	Float4 m3 = Float4::SetZero();
	Float4::Transpose(m0, m1, m2, m3);

	for (int i = 0; i < count; i++)
	{
		Float4 iPos = Float4::Load3(&vertexes[i].Pos);
		Float4 iTangent = Float4::Load3(&vertexes[i].Tangent);
		Float4 iBinormal = Float4::Load3(&vertexes[i].Binormal);

		Float4 oPos = Float4::MulAddLane<0>(m3, m0, iPos);
		oPos = Float4::MulAddLane<1>(oPos, m1, iPos);
		oPos = Float4::MulAddLane<2>(oPos, m2, iPos);

		Float4 oTangent = Float4::MulLane<0>(m0, iTangent);
		oTangent = Float4::MulAddLane<1>(oTangent, m1, iTangent);
		oTangent = Float4::MulAddLane<2>(oTangent, m2, iTangent);

		Float4 oBinormal = Float4::MulLane<0>(m0, iBinormal);
		oBinormal = Float4::MulAddLane<1>(oBinormal, m1, iBinormal);
		oBinormal = Float4::MulAddLane<2>(oBinormal, m2, iBinormal);

		Float4::Store3(&vertexes[i].Pos, oPos);
		Float4::Store3(&vertexes[i].Tangent, oTangent);
		Float4::Store3(&vertexes[i].Binormal, oBinormal);
	}
}

inline void TransformVertexes(StrideView<VertexDistortion>& v, int32_t count, const ::Effekseer::SIMD::Mat43f& mat)
{
	TransformDistortionVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<SimpleVertex>& v, int32_t count, const ::Effekseer::SIMD::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<DynamicVertex>& v, int32_t count, const ::Effekseer::SIMD::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<LightingVertex>& v, int32_t count, const ::Effekseer::SIMD::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<AdvancedVertexDistortion>& v, int32_t count, const ::Effekseer::SIMD::Mat43f& mat)
{
	TransformDistortionVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<AdvancedSimpleVertex>& v, int32_t count, const ::Effekseer::SIMD::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void TransformVertexes(StrideView<AdvancedLightingVertex>& v, int32_t count, const ::Effekseer::SIMD::Mat43f& mat)
{
	TransformStandardVertexes(v, count, mat);
}

inline void SwapRGBAToBGRA(Effekseer::Color& color)
{
	auto temp = color;
	color.B = temp.R;
	color.R = temp.B;
}

inline Effekseer::Color PackVector3DF(const Effekseer::SIMD::Vec3f& v)
{
	Effekseer::Color ret;
	ret.R = static_cast<uint8_t>(Effekseer::Clamp(((v.GetX() + 1.0f) / 2.0f + 0.5f / 255.0f) * 255.0f, 255, 0));
	ret.G = static_cast<uint8_t>(Effekseer::Clamp(((v.GetY() + 1.0f) / 2.0f + 0.5f / 255.0f) * 255.0f, 255, 0));
	ret.B = static_cast<uint8_t>(Effekseer::Clamp(((v.GetZ() + 1.0f) / 2.0f + 0.5f / 255.0f) * 255.0f, 255, 0));
	ret.A = 255;
	return ret;
}

inline Effekseer::SIMD::Vec3f SafeNormalize(const Effekseer::SIMD::Vec3f& v)
{
	auto lengthSq = v.GetSquaredLength();
	auto e = 0.0001f;
	if (lengthSq < e * e)
	{
		return v;
	}

	return v * Effekseer::SIMD::Rsqrt(lengthSq);
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
	int32_t PixelReconstructionParam1Offset = -1;
	int32_t PixelReconstructionParam2Offset = -1;
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
			vsOffset += sizeof(Effekseer::SIMD::Mat44f);

			VertexModelMatrixOffset = vsOffset;
			vsOffset += sizeof(Effekseer::SIMD::Mat44f) * instanceCount;

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
			vsOffset += sizeof(Effekseer::SIMD::Mat44f);

			VertexProjectionMatrixOffset = vsOffset;
			vsOffset += sizeof(Effekseer::SIMD::Mat44f);

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

		PixelReconstructionParam1Offset = psOffset;
		psOffset += sizeof(float) * 4;

		PixelReconstructionParam2Offset = psOffset;
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
			psOffset += sizeof(Effekseer::SIMD::Mat44f);
		}

		PixelUserUniformOffset = psOffset;
		psOffset += sizeof(float) * 4 * material.GetUniformCount();

		PixelShaderUniformBufferSize = psOffset;
	}
};

enum class RendererShaderType
{
	Unlit,
	Lit,
	BackDistortion,
	AdvancedUnlit,
	AdvancedLit,
	AdvancedBackDistortion,
	Material,
};

struct ShaderParameterCollector
{
	RendererShaderType ShaderType{};

	Effekseer::MaterialParameter* MaterialParam = nullptr;
	Effekseer::MaterialData* MaterialDataPtr = nullptr;

	int32_t TextureCount = 0;
	std::array<::Effekseer::TextureData*, Effekseer::TextureSlotMax> Textures;
	std::array<::Effekseer::TextureFilterType, Effekseer::TextureSlotMax> TextureFilterTypes;
	std::array<::Effekseer::TextureWrapType, Effekseer::TextureSlotMax> TextureWrapTypes;

	bool IsDepthRequired = false;
	bool IsBackgroundRequiredOnFirstPass = false;
	bool HasMultiPass = false;
	int32_t BackgroundIndex = -1;
	int32_t DepthIndex = -1;

	bool DoRequireAdvancedRenderer() const
	{
		return ShaderType == RendererShaderType::AdvancedUnlit ||
			   ShaderType == RendererShaderType::AdvancedLit ||
			   ShaderType == RendererShaderType::AdvancedBackDistortion;
	}

	bool operator!=(const ShaderParameterCollector& state) const
	{
		if (ShaderType != state.ShaderType)
			return true;

		if (MaterialParam != state.MaterialParam)
			return true;

		if (MaterialDataPtr != state.MaterialDataPtr)
			return true;

		if (IsBackgroundRequiredOnFirstPass != state.IsBackgroundRequiredOnFirstPass)
			return true;

		if (HasMultiPass != state.HasMultiPass)
			return true;

		if (BackgroundIndex != state.BackgroundIndex)
			return true;

		if (TextureCount != state.TextureCount)
			return true;

		for (int32_t i = 0; i < TextureCount; i++)
		{
			if (Textures[i] != state.Textures[i])
				return true;

			if (TextureFilterTypes[i] != state.TextureFilterTypes[i])
				return true;

			if (TextureWrapTypes[i] != state.TextureWrapTypes[i])
				return true;
		}

		return false;
	}

	void Collect(Renderer* renderer, Effekseer::Effect* effect, Effekseer::NodeRendererBasicParameter* param, bool edgeFalloff, bool isSoftParticleEnabled)
	{
		::Effekseer::TextureData* TexturePtr = nullptr;
		::Effekseer::TextureData* NormalTexturePtr = nullptr;
		::Effekseer::TextureData* AlphaTexturePtr = nullptr;
		::Effekseer::TextureData* UVDistortionTexturePtr = nullptr;
		::Effekseer::TextureData* BlendTexturePtr = nullptr;
		::Effekseer::TextureData* BlendAlphaTexturePtr = nullptr;
		::Effekseer::TextureData* BlendUVDistortionTexturePtr = nullptr;

		Textures.fill(nullptr);
		TextureFilterTypes.fill(::Effekseer::TextureFilterType::Linear);
		TextureWrapTypes.fill(::Effekseer::TextureWrapType::Repeat);

		BackgroundIndex = -1;
		IsBackgroundRequiredOnFirstPass = false;

		DepthIndex = -1;
		IsDepthRequired = isSoftParticleEnabled;

		auto isAdvanced = param->GetIsRenderedWithAdvancedRenderer() || edgeFalloff;

		if (param->MaterialType == ::Effekseer::RendererMaterialType::File)
		{
			MaterialParam = param->MaterialParameterPtr;
			if (MaterialParam != nullptr)
			{
				MaterialDataPtr = effect->GetMaterial(param->MaterialParameterPtr->MaterialIndex);

				if (MaterialDataPtr != nullptr && !MaterialDataPtr->IsSimpleVertex)
				{
					ShaderType = RendererShaderType::Material;
					IsBackgroundRequiredOnFirstPass = MaterialDataPtr->IsRefractionRequired;
					HasMultiPass = true;
				}
				else
				{
					ShaderType = RendererShaderType::Unlit;
					MaterialParam = nullptr;
					MaterialDataPtr = nullptr;
				}

				// Validate parameters
				if (MaterialDataPtr != nullptr && (MaterialDataPtr->TextureCount != MaterialParam->MaterialTextures.size() ||
												   MaterialDataPtr->UniformCount != MaterialParam->MaterialUniforms.size()))
				{
					ShaderType = RendererShaderType::Unlit;
					MaterialParam = nullptr;
					MaterialDataPtr = nullptr;
				}
			}
		}
		else if (param->MaterialType == ::Effekseer::RendererMaterialType::Lighting && isAdvanced)
		{
			ShaderType = RendererShaderType::AdvancedLit;
		}
		else if (param->MaterialType == ::Effekseer::RendererMaterialType::BackDistortion && isAdvanced)
		{
			ShaderType = RendererShaderType::AdvancedBackDistortion;
			IsBackgroundRequiredOnFirstPass = true;
		}
		else if (param->MaterialType == ::Effekseer::RendererMaterialType::Default && isAdvanced)
		{
			ShaderType = RendererShaderType::AdvancedUnlit;
		}
		else if (param->MaterialType == ::Effekseer::RendererMaterialType::Lighting)
		{
			ShaderType = RendererShaderType::Lit;
		}
		else if (param->MaterialType == ::Effekseer::RendererMaterialType::BackDistortion)
		{
			ShaderType = RendererShaderType::BackDistortion;
			IsBackgroundRequiredOnFirstPass = true;
		}
		else if (param->MaterialType == ::Effekseer::RendererMaterialType::Default)
		{
			ShaderType = RendererShaderType::Unlit;
		}
		else
		{
			assert(0);
		}

		if (MaterialParam != nullptr && MaterialDataPtr != nullptr)
		{
			TextureCount = static_cast<int32_t>(Effekseer::Min(MaterialParam->MaterialTextures.size(), ::Effekseer::UserTextureSlotMax));
			for (size_t i = 0; i < TextureCount; i++)
			{
				if (MaterialParam->MaterialTextures[i].Type == 1)
				{
					if (MaterialParam->MaterialTextures[i].Index >= 0)
					{
						Textures[i] = effect->GetNormalImage(MaterialParam->MaterialTextures[i].Index);
					}
					else
					{
						Textures[i] = nullptr;
					}
				}
				else
				{
					if (MaterialParam->MaterialTextures[i].Index >= 0)
					{
						Textures[i] = effect->GetColorImage(MaterialParam->MaterialTextures[i].Index);
					}
					else
					{
						Textures[i] = nullptr;
					}
				}

				TextureFilterTypes[i] = Effekseer::TextureFilterType::Linear;
				TextureWrapTypes[i] = MaterialDataPtr->TextureWrapTypes[i];
			}

			if (IsBackgroundRequiredOnFirstPass)
			{
				// Store from external
				TextureFilterTypes[TextureCount] = Effekseer::TextureFilterType::Linear;
				TextureWrapTypes[TextureCount] = Effekseer::TextureWrapType::Clamp;
				BackgroundIndex = TextureCount;
			}
			TextureCount += 1;

			if (IsDepthRequired)
			{
				// Store from external
				TextureFilterTypes[TextureCount] = Effekseer::TextureFilterType::Linear;
				TextureWrapTypes[TextureCount] = Effekseer::TextureWrapType::Clamp;
				DepthIndex = TextureCount;
				TextureCount += 1;			
			}
		}
		else
		{
			if (isAdvanced)
			{
				if (param->MaterialType == ::Effekseer::RendererMaterialType::Default)
				{
					TextureCount = 6;
				}
				else
				{
					TextureCount = 7;
				}

				if (IsDepthRequired)
				{
					DepthIndex = TextureCount;
					TextureCount += 1;
				}
			}
			else
			{
				if (param->MaterialType == ::Effekseer::RendererMaterialType::Default)
				{
					TextureCount = 1;
				}
				else
				{
					TextureCount = 2;
				}

				if (IsDepthRequired)
				{
					DepthIndex = TextureCount;
					TextureCount += 1;
				}
			}

			// color/distortion
			if (param->MaterialType == ::Effekseer::RendererMaterialType::BackDistortion)
			{
				TexturePtr = effect->GetDistortionImage(param->Texture1Index);
			}
			else
			{
				TexturePtr = effect->GetColorImage(param->Texture1Index);
			}

			if (TexturePtr == nullptr && renderer != nullptr)
			{
				TexturePtr = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
			}

			Textures[0] = TexturePtr;
			TextureFilterTypes[0] = param->TextureFilter1;
			TextureWrapTypes[0] = param->TextureWrap1;

			// normal/background
			if (param->MaterialType != ::Effekseer::RendererMaterialType::Default)
			{
				if (param->MaterialType == ::Effekseer::RendererMaterialType::BackDistortion)
				{
					// Store from external
					IsBackgroundRequiredOnFirstPass = true;
					BackgroundIndex = 1;
				}
				else if (param->MaterialType == ::Effekseer::RendererMaterialType::Lighting)
				{
					NormalTexturePtr = effect->GetNormalImage(param->Texture2Index);

					if (NormalTexturePtr == nullptr && renderer != nullptr)
					{
						NormalTexturePtr = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
					}

					Textures[1] = NormalTexturePtr;
				}

				if (param->MaterialType == ::Effekseer::RendererMaterialType::BackDistortion)
				{
					TextureFilterTypes[1] = Effekseer::TextureFilterType::Linear;
					TextureWrapTypes[1] = Effekseer::TextureWrapType::Clamp;
				}
				else
				{
					TextureFilterTypes[1] = param->TextureFilter2;
					TextureWrapTypes[1] = param->TextureWrap2;
				}
			}

			if (isAdvanced)
			{
				if (param->MaterialType == ::Effekseer::RendererMaterialType::BackDistortion)
				{
					AlphaTexturePtr = effect->GetDistortionImage(param->Texture3Index);
				}
				else
				{
					AlphaTexturePtr = effect->GetColorImage(param->Texture3Index);
				}

				if (AlphaTexturePtr == nullptr && renderer != nullptr)
				{
					AlphaTexturePtr = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (param->MaterialType == ::Effekseer::RendererMaterialType::BackDistortion)
				{
					UVDistortionTexturePtr = effect->GetDistortionImage(param->Texture4Index);
				}
				else
				{
					UVDistortionTexturePtr = effect->GetColorImage(param->Texture4Index);
				}

				if (UVDistortionTexturePtr == nullptr && renderer != nullptr)
				{
					UVDistortionTexturePtr = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				if (param->MaterialType == ::Effekseer::RendererMaterialType::BackDistortion)
				{
					BlendTexturePtr = effect->GetDistortionImage(param->Texture5Index);
				}
				else
				{
					BlendTexturePtr = effect->GetColorImage(param->Texture5Index);
				}

				if (BlendTexturePtr == nullptr && renderer != nullptr)
				{
					BlendTexturePtr = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (param->MaterialType == ::Effekseer::RendererMaterialType::BackDistortion)
				{
					BlendAlphaTexturePtr = effect->GetDistortionImage(param->Texture6Index);
				}
				else
				{
					BlendAlphaTexturePtr = effect->GetColorImage(param->Texture6Index);
				}

				if (BlendAlphaTexturePtr == nullptr && renderer != nullptr)
				{
					BlendAlphaTexturePtr = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (param->MaterialType == ::Effekseer::RendererMaterialType::BackDistortion)
				{
					BlendUVDistortionTexturePtr = effect->GetDistortionImage(param->Texture7Index);
				}
				else
				{
					BlendUVDistortionTexturePtr = effect->GetColorImage(param->Texture7Index);
				}

				if (BlendUVDistortionTexturePtr == nullptr && renderer != nullptr)
				{
					BlendUVDistortionTexturePtr = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				int offset = 1;

				if (param->MaterialType != ::Effekseer::RendererMaterialType::Default)
				{
					offset += 1;
				}

				Textures[offset + 0] = AlphaTexturePtr;
				TextureFilterTypes[offset + 0] = param->TextureFilter3;
				TextureWrapTypes[offset + 0] = param->TextureWrap3;

				Textures[offset + 1] = UVDistortionTexturePtr;
				TextureFilterTypes[offset + 1] = param->TextureFilter4;
				TextureWrapTypes[offset + 1] = param->TextureWrap4;

				Textures[offset + 2] = BlendTexturePtr;
				TextureFilterTypes[offset + 2] = param->TextureFilter5;
				TextureWrapTypes[offset + 2] = param->TextureWrap5;

				Textures[offset + 3] = BlendAlphaTexturePtr;
				TextureFilterTypes[offset + 3] = param->TextureFilter6;
				TextureWrapTypes[offset + 3] = param->TextureWrap6;

				Textures[offset + 4] = BlendUVDistortionTexturePtr;
				TextureFilterTypes[offset + 4] = param->TextureFilter7;
				TextureWrapTypes[offset + 4] = param->TextureWrap7;
			}
		}
	}
};

struct SoftParticleParameter
{
	std::array<float, 4> softParticleAndReconstructionParam1; // x:softparticle y:reconstruction
	std::array<float, 4> reconstructionParam2;

	void SetParam(float softParticle, float magnification, float rescale1, float rescale2, float v33, float v34, float v43, float v44)
	{
		softParticleAndReconstructionParam1[0] = softParticle * magnification;
		softParticleAndReconstructionParam1[1] = rescale1;
		softParticleAndReconstructionParam1[2] = rescale2;
		softParticleAndReconstructionParam1[3] = magnification;

		reconstructionParam2[0] = v33;
		reconstructionParam2[1] = v34;
		reconstructionParam2[2] = v43;
		reconstructionParam2[3] = v44;
	}
};

struct FlipbookParameter
{
	union {
		float Buffer[4];

		struct
		{
			float EnableInterpolation;
			float InterpolationType;
		};
	};
};

struct UVDistortionParameter
{
	union {
		float Buffer[4];

		struct
		{
			float Intensity;
			float BlendIntensity;
			float UVInversed[2];
		};
	};
};

struct BlendTextureParameter
{
	union {
		float Buffer[4];

		struct
		{
			float BlendType;
		};
	};
};

struct EmmisiveParameter
{
	union {
		float Buffer[4];

		struct
		{
			float EmissiveScaling;
		};
	};
};

struct EdgeParameter
{
	float EdgeColor[4];

	union {
		float Buffer[4];

		struct
		{
			float Threshold;
			float ColorScaling;
		};
	};
};

struct FalloffParameter
{
	union {
		float Buffer[4];

		struct
		{
			float Enable;
			float ColorBlendType;
			float Pow;
		};
	};

	float BeginColor[4];
	float EndColor[4];
};

} // namespace EffekseerRenderer
#endif // __EFFEKSEERRENDERER_COMMON_UTILS_H__
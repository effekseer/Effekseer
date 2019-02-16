
#ifndef	__EFFEKSEERRENDERER_COMMON_UTILS_H__
#define	__EFFEKSEERRENDERER_COMMON_UTILS_H__

#include <Effekseer.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <array>

#if defined(_M_IX86) || defined(__x86__)
#define EFK_SSE2
#include <emmintrin.h>
#elif defined(__ARM_NEON__)
#define EFK_NEON
#include <arm_neon.h>
#endif

#ifdef _MSC_VER
#include <xmmintrin.h>
#endif

namespace EffekseerRenderer
{

void ApplyDepthOffset(::Effekseer::Matrix43& mat, const ::Effekseer::Vector3D& cameraFront, const ::Effekseer::Vector3D& cameraPos, float depthOffset, bool isDepthOffsetScaledWithCamera, bool isDepthOffsetScaledWithEffect, bool isRightHand);

void ApplyDepthOffset(::Effekseer::Matrix43& mat, const ::Effekseer::Vector3D& cameraFront, const ::Effekseer::Vector3D& cameraPos, ::Effekseer::Vector3D& scaleValues, float depthOffset, bool isDepthOffsetScaledWithCamera, bool isDepthOffsetScaledWithEffect, bool isRightHand);

void ApplyDepthOffset(::Effekseer::Matrix44& mat, const ::Effekseer::Vector3D& cameraFront, const ::Effekseer::Vector3D& cameraPos, float depthOffset, bool isDepthOffsetScaledWithCamera, bool isDepthOffsetScaledWithEffect, bool isRightHand);

template <typename Vertex>
inline void TransformStandardVertexes( Vertex* vertexes, int32_t count, const ::Effekseer::Matrix43& mat )
{
	alignas(16) float Value3[4] = {mat.Value[3][0], mat.Value[3][1], mat.Value[3][2], 0.0f};
#if defined(EFK_SSE2)
	__m128 r0 = _mm_loadu_ps( mat.Value[0] );
	__m128 r1 = _mm_loadu_ps( mat.Value[1] );
	__m128 r2 = _mm_loadu_ps( mat.Value[2] );
	__m128 r3 = _mm_load_ps( Value3 );

	float tmp_out[4];
	::Effekseer::Vector3D* inout_prev;

	// 1st loop
	{
		::Effekseer::Vector3D* inout_cur = &vertexes[0].Pos;
		__m128 v = _mm_loadu_ps( (const float*)inout_cur );

		__m128 x = _mm_shuffle_ps( v, v, _MM_SHUFFLE(0,0,0,0) );
		__m128 a0 = _mm_mul_ps( r0, x );
		__m128 y = _mm_shuffle_ps( v, v, _MM_SHUFFLE(1,1,1,1) );
		__m128 a1 = _mm_mul_ps( r1, y );
		__m128 z = _mm_shuffle_ps( v, v, _MM_SHUFFLE(2,2,2,2) );
		__m128 a2 = _mm_mul_ps( r2, z );

		__m128 a01 = _mm_add_ps( a0, a1 );
		__m128 a23 = _mm_add_ps( a2, r3 );
		__m128 a = _mm_add_ps( a01, a23 );

		// store the result of 1st loop
		_mm_storeu_ps( tmp_out, a );
		inout_prev = inout_cur;
	}

	for( int i = 1; i < count; i++ )
	{
		::Effekseer::Vector3D* inout_cur = &vertexes[i].Pos;
		__m128 v = _mm_loadu_ps( (const float*)inout_cur );

		__m128 x = _mm_shuffle_ps( v, v, _MM_SHUFFLE(0,0,0,0) );
		__m128 a0 = _mm_mul_ps( r0, x );
		__m128 y = _mm_shuffle_ps( v, v, _MM_SHUFFLE(1,1,1,1) );
		__m128 a1 = _mm_mul_ps( r1, y );
		__m128 z = _mm_shuffle_ps( v, v, _MM_SHUFFLE(2,2,2,2) );
		__m128 a2 = _mm_mul_ps( r2, z );

		__m128 a01 = _mm_add_ps( a0, a1 );
		__m128 a23 = _mm_add_ps( a2, r3 );
		__m128 a = _mm_add_ps( a01, a23 );

		// write the result of previous loop
		inout_prev->X = tmp_out[0];
		inout_prev->Y = tmp_out[1];
		inout_prev->Z = tmp_out[2];

		// store the result of current loop
		_mm_storeu_ps( tmp_out, a );
		inout_prev = inout_cur;
	}

	// write the result of last loop
	{
		inout_prev->X = tmp_out[0];
		inout_prev->Y = tmp_out[1];
		inout_prev->Z = tmp_out[2];
	}
#elif defined(EFK_NEON)
	float32x4_t r0 = vld1q_f32( mat.Value[0] );
	float32x4_t r1 = vld1q_f32( mat.Value[1] );
	float32x4_t r2 = vld1q_f32( mat.Value[2] );
	float32x4_t r3 = vld1q_f32( Value3 );

	float tmp_out[4];
	::Effekseer::Vector3D* inout_prev;

	// 1st loop
	{
		::Effekseer::Vector3D* inout_cur = &vertexes[0].Pos;
		float32x4_t v = vld1q_f32( (const float*)inout_cur );

		float32x4_t a = vmlaq_lane_f32( r3, r0, vget_low_f32(v), 0 );
		a = vmlaq_lane_f32( a, r1, vget_low_f32(v), 1 );
		a = vmlaq_lane_f32( a, r2, vget_high_f32(v), 0 );

		// store the result of 1st loop
		vst1q_f32( tmp_out, a );
		inout_prev = inout_cur;
	}

	for( int i = 1; i < count; i++ )
	{
		::Effekseer::Vector3D* inout_cur = &vertexes[i].Pos;
		float32x4_t v = vld1q_f32( (const float*)inout_cur );

		float32x4_t a = vmlaq_lane_f32( r3, r0, vget_low_f32(v), 0 );
		a = vmlaq_lane_f32( a, r1, vget_low_f32(v), 1 );
		a = vmlaq_lane_f32( a, r2, vget_high_f32(v), 0 );

		// write the result of previous loop
		inout_prev->X = tmp_out[0];
		inout_prev->Y = tmp_out[1];
		inout_prev->Z = tmp_out[2];

		// store the result of current loop
		vst1q_f32( tmp_out, a );
		inout_prev = inout_cur;
	}

	// write the result of last loop
	{
		inout_prev->X = tmp_out[0];
		inout_prev->Y = tmp_out[1];
		inout_prev->Z = tmp_out[2];
	}
#else
	for( int i = 0; i < count; i++ )
	{
		::Effekseer::Vector3D::Transform(
			vertexes[i].Pos,
			vertexes[i].Pos,
			mat );
	}
#endif
}

template <typename VertexDistortion>
inline void TransformDistortionVertexes(VertexDistortion* vertexes, int32_t count, const ::Effekseer::Matrix43& mat)
{
	TransformStandardVertexes( vertexes, count, mat );

	for (int i = 0; i < count; i++)
	{
		auto vs = &vertexes[i];

		::Effekseer::Vector3D::Transform(
			vs->Tangent,
			vs->Tangent,
			mat);

		::Effekseer::Vector3D::Transform(
			vs->Binormal,
			vs->Binormal,
			mat);

		Effekseer::Vector3D zero;
		::Effekseer::Vector3D::Transform(
			zero,
			zero,
			mat);

		::Effekseer::Vector3D::Normal(vs->Tangent, vs->Tangent - zero);
		::Effekseer::Vector3D::Normal(vs->Binormal, vs->Binormal - zero);
	}
}

template <typename T>
struct HasDistortion
{
private:
	template <typename U>
	static auto check_has_dist(U v) -> decltype(v.Normal, std::true_type{});
	static std::false_type check_has_dist(...);
public:
	static bool const value = decltype(check_has_dist(std::declval<T>()))::value;
};

template <typename Vertex, 
	typename std::enable_if<!HasDistortion<Vertex>::value>::type* = nullptr>
void TransformVertexes(Vertex& v, int32_t count, const ::Effekseer::Matrix43& mat)
{
	TransformStandardVertexes(v, count, mat);
}

template <typename Vertex, 
	typename std::enable_if<HasDistortion<Vertex>::value>::type* = nullptr>
void TransformVertexes(Vertex& v, int32_t count, const ::Effekseer::Matrix43& mat)
{
	TransformDistortionVertexes(v, count, mat);
}

}
#endif // __EFFEKSEERRENDERER_COMMON_UTILS_H__
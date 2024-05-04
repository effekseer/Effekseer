
#ifndef __EFFEKSEER_POINT_CACHE_GENERATOR_H__
#define __EFFEKSEER_POINT_CACHE_GENERATOR_H__

#include "../Utils/Effekseer.CustomAllocator.h"
#include <cstdint>
#include <vector>

namespace Effekseer
{

/**
	@brief PointCache generator
*/
class PointCacheGenerator
{
public:
	struct Point
	{
		Vector3D Position;
	};
	// (fp32-x3) Point
	void SetPointBuffer(void* buffer, size_t stride);
	
	struct Attribute
	{
		uint32_t PackedNormal;
		uint32_t PackedTangent;
		uint32_t PackedUV;
		uint32_t PackedColor;
	};
	// (Packed32-x4) Normal, Tangent, UV, Color
	void SetAttributeBuffer(void* buffer, size_t stride);

	void SetSourceModel(ModelRef model);

	void Generate(uint32_t pointCount, uint32_t seed);

private:
	uintptr_t pointBuffer_{};
	size_t pointStride_{};
	uintptr_t attribBuffer_{};
	size_t attribStride_{};

	ModelRef model_;
	CustomVector<CustomVector<float>> modelFaceAreas_;
	float totalArea_ = 0.0f;

};

} // namespace Effekseer

#endif

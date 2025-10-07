
#ifndef __EFFEKSEER_DYNAMIC_PARAMETER_H__
#define __EFFEKSEER_DYNAMIC_PARAMETER_H__

#include "../Effekseer.Base.Pre.h"
#include "../Effekseer.InternalStruct.h"

namespace Effekseer
{

/**
	@brief	a factor to calculate original parameter for dynamic parameter
*/
struct DynamicFactorParameter
{
	std::array<float, 3> Tra;
	std::array<float, 3> TraInv;
	std::array<float, 3> Rot;
	std::array<float, 3> RotInv;
	std::array<float, 3> Scale;
	std::array<float, 3> ScaleInv;

	DynamicFactorParameter()
	{
		Tra.fill(1.0f);
		TraInv.fill(1.0f);
		Rot.fill(1.0f);
		RotInv.fill(1.0f);
		Scale.fill(1.0f);
		ScaleInv.fill(1.0f);
	}
};

/**!
	@brief indexes of dynamic parameter
*/
struct RefMinMax
{
	int32_t Max = -1;
	int32_t Min = -1;
};

//! calculate dynamic equation and assign a result
void ApplyEq(float& dstParam, const Effect* e, const InstanceGlobal* instg, const Instance* parrentInstance, IRandObject* rand, int dpInd, const float& originalParam);

//! calculate dynamic equation and return a result
SIMD::Vec3f ApplyEq(const Effect* e,
					const InstanceGlobal* instg,
					const Instance* parrentInstance,
					IRandObject* rand,
					const int& dpInd,
					const SIMD::Vec3f& originalParam,
					const std::array<float, 3>& scale,
					const std::array<float, 3>& scaleInv);

//! calculate dynamic equation and return a result
random_float ApplyEq(const Effect* e, const InstanceGlobal* instg, const Instance* parrentInstance, IRandObject* rand, const RefMinMax& dpInd, random_float originalParam);

//! calculate dynamic equation and return a result
random_vector3d ApplyEq(const Effect* e,
						const InstanceGlobal* instg,
						const Instance* parrentInstance,
						IRandObject* rand,
						const RefMinMax& dpInd,
						random_vector3d originalParam,
						const std::array<float, 3>& scale,
						const std::array<float, 3>& scaleInv);

//! calculate dynamic equation and return a result
random_int ApplyEq(const Effect* e, const InstanceGlobal* instg, const Instance* parrentInstance, IRandObject* rand, const RefMinMax& dpInd, random_int originalParam);

} // namespace Effekseer

#endif // __EFFEKSEER_PARAMETERS_H__
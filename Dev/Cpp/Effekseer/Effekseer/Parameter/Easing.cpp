#include "Easing.h"

namespace Effekseer
{


float ParameterEasingFloat::GetValue(const InstanceEasingType& instance, float time)
{
	auto t = getEaseValue(type_, time);

	if (isIndividualEnabled)
	{
		t = getEaseValue(types[0], time);
	}

	if (isMiddleEnabled)
	{
		return get3Point(instance, t);
	}

	return get2Point(instance, t);
}

void ParameterEasingFloat::Init(InstanceEasingType& instance, Effect* e, InstanceGlobal* instg, Instance* parent, IRandObject* rand)
{
	auto rvs = ApplyEq(e,
					   instg,
					   parent,
					   rand,
					   RefEqS,
					   start);
	auto rve = ApplyEq(e,
					   instg,
					   parent,
					   rand,
					   RefEqE,
					   end);

	instance.start = rvs.getValue(*rand);
	instance.end = rve.getValue(*rand);

	if (isMiddleEnabled)
	{
		auto rvm = ApplyEq(e,
						   instg,
						   parent,
						   rand,
						   RefEqM,
						   middle);

		instance.middle = rvm.getValue(*rand);
	}
}




Vec3f ParameterEasingVec3f::GetValue(const InstanceEasingType& instance, float time)
{
	if (isIndividualEnabled)
	{
		std::array<Vec3f, 3> values;

		for (size_t i = 0; i < 3; i++)
		{
			auto t = getEaseValue(types[i], time);

			if (isMiddleEnabled)
			{
				values[i] = get3Point(instance, t);
			}
			else
			{
				values[i] = get2Point(instance, t);
			}
		}

		return Vec3f(values[0].GetX(), values[1].GetY(), values[2].GetZ());
	}
	else
	{
		auto t = getEaseValue(type_, time);

		if (isMiddleEnabled)
		{
			return get3Point(instance, t);
		}

		return get2Point(instance, t);
	}
}

void ParameterEasingVec3f::Init(InstanceEasingType& instance, Effect* e, InstanceGlobal* instg, Instance* parent, IRandObject* rand, const std::array<float, 3>& scale, const std::array<float, 3>& scaleInv)
{
	auto rvs = ApplyEq(e,
					   instg,
					   parent,
					   rand,
					   RefEqS,
					   start,
					   scale,
					   scaleInv);
	auto rve = ApplyEq(e,
					   instg,
					   parent,
					   rand,
					   RefEqE,
					   end,
					   scale,
					   scaleInv);

	instance.start = rvs.getValue(channelIDs, channelCount, *rand);
	instance.end = rve.getValue(channelIDs, channelCount, *rand);

	if (isMiddleEnabled)
	{
		auto rvm = ApplyEq(e,
						   instg,
						   parent,
						   rand,
						   RefEqM,
						   middle,
						   scale,
						   scaleInv);

		instance.middle = rvm.getValue(channelIDs, channelCount, *rand);
	}
}




}

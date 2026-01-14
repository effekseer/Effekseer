#include "EfkRes.EfkCurveSaver.h"
#include "EfkRes.Utils.h"
#include <algorithm>

namespace efkres
{

namespace
{

}

bool EfkCurveSaver::Save(std::string_view filepath, const Curve& curve)
{
	const int32_t version = 1;

	BinaryWriter writer;
	if (!writer.Open(std::string(filepath).c_str()))
	{
		return false;
	}

	writer.Write(version);

	writer.Write(static_cast<uint32_t>(curve.controllPoints.size()));

	for (auto& point : curve.controllPoints)
	{
		writer.Write(&point, sizeof(double) * 4);
	}

	writer.Write(static_cast<uint32_t>(curve.knotValues.size()));

	for (auto& knot : curve.knotValues)
	{
		writer.Write(&knot, sizeof(double));
	}

	writer.Write(curve.order);
	writer.Write(curve.step);
	writer.Write(curve.type);
	writer.Write(curve.dimension);

	return true;
}

} // namespace efkres

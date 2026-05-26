#include <Effekseer/Effekseer.FCurves.h>

#include "../TestHelper.h"

namespace
{

template <class T>
void Append(std::vector<uint8_t>& data, const T& value)
{
	const auto* bytes = reinterpret_cast<const uint8_t*>(&value);
	data.insert(data.end(), bytes, bytes + sizeof(T));
}

std::vector<uint8_t> CreateFCurveData(int32_t len, int32_t freq, const std::vector<float>& keys)
{
	std::vector<uint8_t> data;

	const int32_t edgeConstant = 0;
	const float offsetMax = 0.0f;
	const float offsetMin = 0.0f;
	const int32_t offset = 0;
	const auto count = static_cast<int32_t>(keys.size());

	Append(data, edgeConstant);
	Append(data, edgeConstant);
	Append(data, offsetMax);
	Append(data, offsetMin);
	Append(data, offset);
	Append(data, len);
	Append(data, freq);
	Append(data, count);

	for (auto key : keys)
	{
		Append(data, key);
	}

	return data;
}

void TestFCurveLinearLastSegment()
{
	const std::vector<float> keys = {
		0.0f,
		10.0f,
		20.0f,
		30.0f,
		40.0f,
		50.0f,
		60.0f,
		70.0f,
		80.0f,
		90.0f,
		95.0f,
	};
	const auto data = CreateFCurveData(95, 10, keys);

	Effekseer::FCurve curve(0.0f);
	curve.Load(data.data(), 1800);

	EXPECT_EQUAL_NEAR(curve.GetValue(90.0f, 1.0f, Effekseer::FCurveTimelineType::Time), 90.0f, 0.0001f);
	EXPECT_EQUAL_NEAR(curve.GetValue(94.0f, 1.0f, Effekseer::FCurveTimelineType::Time), 94.0f, 0.0001f);
	EXPECT_EQUAL_NEAR(curve.GetValue(95.0f, 1.0f, Effekseer::FCurveTimelineType::Time), 95.0f, 0.0001f);
}

} // namespace

TestRegister Runtime_FCurveLinearLastSegment("Runtime.FCurve.LinearLastSegment", []() -> void
											{ TestFCurveLinearLastSegment(); });

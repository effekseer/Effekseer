#include "../TestHelper.h"
#include <Effekseer.h>
#include <Effekseer/Effekseer.FCurves.h>
#include <Effekseer/IO/Effekseer.EfkEfcFactory.h>
#include <Effekseer/Parameter/Effekseer.Rotation.h>
#include <array>
#include <cstring>
#include <limits>

namespace
{
void AppendInt(std::vector<uint8_t>& data, int32_t value)
{
	const auto offset = data.size();
	data.resize(offset + sizeof(value));
	memcpy(data.data() + offset, &value, sizeof(value));
}

void ExpectRejected(const std::vector<uint8_t>& data)
{
	auto setting = Effekseer::Setting::Create();
	EXPECT_TRUE(Effekseer::Effect::Create(setting, data.data(), static_cast<int32_t>(data.size())) == nullptr);
}

void TestNodeBounds()
{
	// Version 1810 header with no resources or node data.
	std::vector<uint8_t> header{'S', 'K', 'F', 'E'};
	AppendInt(header, 1810);
	header.resize(80, 0);
	ExpectRejected(header);

	for (int32_t type : {-1, 0, 2, 3, 4, 5, 6, 12345})
	{
		auto data = header;
		AppendInt(data, type);
		ExpectRejected(data);
	}

	for (int32_t count : {-1, 1, 1025, std::numeric_limits<int32_t>::max()})
	{
		auto data = header;
		AppendInt(data, -1);
		AppendInt(data, count);
		ExpectRejected(data);
	}

	auto valid = header;
	AppendInt(valid, -1);
	AppendInt(valid, 0);
	EXPECT_TRUE(Effekseer::Effect::Create(Effekseer::Setting::Create(), valid.data(), static_cast<int32_t>(valid.size())) != nullptr);

	auto deep = header;
	for (int i = 0; i < 300; i++)
	{
		AppendInt(deep, -1);
		AppendInt(deep, i == 299 ? 0 : 1);
	}
	ExpectRejected(deep);
}

void TestTruncatedEffects()
{
	const auto base = GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/";
	auto setting = Effekseer::Setting::Create();
	for (const auto* name : {u"10/FCurve_Parameters1.efk", u"10/Ribbon_Parameters1.efk", u"10/Ring_Parameters1.efk", u"10/Sprite_Parameters1.efk", u"10/Track_Parameters1.efk", u"14/Model_Parameters1.efk", u"15/Update_Easing.efkefc", u"16/ForceFieldLocal03.efkefc", u"Update_17x/Sprite.efkefc", u"18/Collisions.efkefc", u"18/GpuParticles_emit_mesh.efkefc", u"18/Materials1.efkefc"})
	{
		std::string label;
		for (const auto* c = name; *c; ++c)
			label += static_cast<char>(*c);
		printf("Checking %s\n", label.c_str());
		auto data = LoadFile((base + name).c_str());
		EXPECT_TRUE(!data.empty());
		Effekseer::EfkEfcFile container(data.data(), static_cast<int32_t>(data.size()));
		if (container.IsValid())
		{
			const auto chunk = container.ReadRuntimeData();
			EXPECT_TRUE(chunk.data != nullptr);
			const auto* begin = static_cast<const uint8_t*>(chunk.data);
			data = std::vector<uint8_t>(begin, begin + chunk.size);
		}
		EXPECT_TRUE(Effekseer::Effect::Create(setting, data.data(), static_cast<int32_t>(data.size())) != nullptr);
		for (size_t size = 0; size < data.size(); size++)
		{
			// Allocate exactly the truncated size so ASan detects reads past the supplied buffer.
			std::vector<uint8_t> truncated(data.begin(), data.begin() + size);
			EXPECT_TRUE(Effekseer::Effect::Create(setting, truncated.data(), static_cast<int32_t>(truncated.size())) == nullptr);
		}
	}
}

void TestInvalidParameterLengths()
{
	for (int32_t count : {-1, 1, 65537, std::numeric_limits<int32_t>::max()})
	{
		std::array<int32_t, 8> fields{};
		fields[6] = 1; // Sampling frequency
		fields[7] = count;
		Effekseer::BinaryReader<true> reader(reinterpret_cast<const uint8_t*>(fields.data()), sizeof(fields));
		Effekseer::FCurve curve(0);
		EXPECT_TRUE(curve.Load(reader, 10) < 0);
	}

	for (int32_t size : {-1, 0, 1024, std::numeric_limits<int32_t>::max()})
	{
		std::vector<uint8_t> data;
		AppendInt(data, static_cast<int32_t>(Effekseer::ParameterRotationType::ParameterRotationType_Fixed));
		AppendInt(data, size);
		Effekseer::BinaryReader<true> reader(data.data(), data.size());
		Effekseer::RotationParameter parameter;
		parameter.Load(reader, 14);
		EXPECT_TRUE(reader.GetStatus() == Effekseer::BinaryReaderStatus::Failed);
	}
}

TestRegister nodeBounds("Runtime.EffectLoading.NodeBounds", TestNodeBounds);
TestRegister truncatedEffects("Runtime.EffectLoading.TruncatedEffects", TestTruncatedEffects);
TestRegister invalidLengths("Runtime.EffectLoading.InvalidParameterLengths", TestInvalidParameterLengths);
} // namespace

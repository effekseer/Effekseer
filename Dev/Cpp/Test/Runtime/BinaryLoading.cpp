#include "../../Effekseer/Effekseer/Effekseer.InternalScript.h"
#include "../../Effekseer/Effekseer/IO/Effekseer.EfkEfcFactory.h"
#include "../../Effekseer/Effekseer/Material/Effekseer.CompiledMaterial.h"
#include "../../Effekseer/Effekseer/Material/Effekseer.MaterialFile.h"
#include "../../Effekseer/Effekseer/Parameter/Effekseer.Parameters.h"
#include "../../EffekseerRendererCommon/EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.h"
#include "../../EffekseerRendererCommon/EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#include "../../EffekseerRendererCommon/EffekseerRendererCommon/EffekseerRenderer.TGATextureLoader.h"
#include "../TestHelper.h"
#include <Effekseer.h>
#include <array>
#include <cstring>
#include <limits>
#include <vector>

namespace
{
template <typename T>
void Append(std::vector<uint8_t>& data, const T& value)
{
	const auto offset = data.size();
	data.resize(offset + sizeof(T));
	memcpy(data.data() + offset, &value, sizeof(T));
}

void TestEfkEfcChunkBounds()
{
	std::vector<uint8_t> data;
	const std::array<char, 4> header{'E', 'F', 'K', 'E'};
	const std::array<char, 4> chunk{'B', 'I', 'N', '_'};
	data.insert(data.end(), header.begin(), header.end());
	Append(data, int32_t{1});
	data.insert(data.end(), chunk.begin(), chunk.end());
	Append(data, int32_t{1024});

	Effekseer::EfkEfcFile file(data.data(), static_cast<int32_t>(data.size()));
	EXPECT_TRUE(file.IsValid());
	EXPECT_TRUE(file.ReadRuntimeData().data == nullptr);
	EXPECT_TRUE(!Effekseer::EfkEfcProperty().Load(data.data(), static_cast<int32_t>(data.size())));
}

void TestGradientBounds()
{
	std::vector<uint8_t> data;
	Append(data, int32_t{Effekseer::Gradient::KeyMax + 1});
	Effekseer::BinaryReader<true> reader(data.data(), data.size());
	Effekseer::Gradient gradient;
	EXPECT_TRUE(!Effekseer::LoadGradient(gradient, reader, 0));
	EXPECT_TRUE(gradient.ColorCount == 0 && gradient.AlphaCount == 0);
}

void TestOtherBinaryBounds()
{
	const std::array<uint8_t, 4> tiny{};
	Effekseer::MaterialFile material;
	Effekseer::CompiledMaterial compiledMaterial;
	Effekseer::VectorFieldLoader vectorFieldLoader;
	EXPECT_TRUE(!material.Load(tiny.data(), static_cast<int32_t>(tiny.size())));
	EXPECT_TRUE(!compiledMaterial.Load(tiny.data(), static_cast<int32_t>(tiny.size())));
	EXPECT_TRUE(vectorFieldLoader.Load(tiny.data(), static_cast<int32_t>(tiny.size())) == nullptr);

	std::vector<uint8_t> script;
	Append(script, int32_t{0});
	Append(script, int32_t{0});
	Append(script, int32_t{65537});
	Append(script, int32_t{0});
	for (int i = 0; i < 4; i++)
		Append(script, int32_t{0});
	Effekseer::InternalScript internalScript;
	EXPECT_TRUE(!internalScript.Load(script.data(), static_cast<int32_t>(script.size())));

	EffekseerRenderer::TGATextureLoader tga;
	EffekseerRenderer::DDSTextureLoader dds;
	EffekseerRenderer::PngTextureLoader png;
	EXPECT_TRUE(!tga.Load(tiny.data(), static_cast<int32_t>(tiny.size())));
	EXPECT_TRUE(!dds.Load(tiny.data(), static_cast<int32_t>(tiny.size())));
	EXPECT_TRUE(!png.Load(tiny.data(), static_cast<int32_t>(tiny.size()), false));
}

void TestValidMaterial()
{
	const auto path = GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/Materials/WorldPositionOffset.efkmat";
	auto data = LoadFile(path.c_str());
	Effekseer::MaterialFile material;
	EXPECT_TRUE(!data.empty());
	EXPECT_TRUE(material.Load(data.data(), static_cast<int32_t>(data.size())));

	std::vector<uint8_t> originalData{1, 2, 3, 4};
	std::vector<uint8_t> compiledData;
	Effekseer::CompiledMaterial source;
	source.Save(compiledData, 1, originalData);
	Effekseer::CompiledMaterial loaded;
	EXPECT_TRUE(loaded.Load(compiledData.data(), static_cast<int32_t>(compiledData.size())));
	EXPECT_TRUE(loaded.GetOriginalData() == originalData);
}

void TestValidEffect()
{
	auto setting = Effekseer::Setting::Create();
	const auto basePath = GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/";
	for (const auto* relativePath : {
			 u"10/Distortions1.efk",
			 u"10/FCurve_Parameters1.efk",
			 u"10/Parents1.efk",
			 u"10/Ribbon_Parameters1.efk",
			 u"10/Ring_Parameters1.efk",
			 u"10/SimpleLaser.efk",
			 u"10/Sprite_Parameters1.efk",
			 u"10/Track_Parameters1.efk",
			 u"14/Model_Parameters1.efk",
			 u"15/Update_Easing.efkefc",
			 u"16/ForceFieldLocal03.efkefc",
			 u"Update_17x/Sprite.efkefc",
			 u"18/Collisions.efkefc"})
	{
		auto data = LoadFile((basePath + relativePath).c_str());
		EXPECT_TRUE(!data.empty());
		EXPECT_TRUE(Effekseer::Effect::Create(setting, data.data(), static_cast<int32_t>(data.size())) != nullptr);
	}
}

TestRegister Runtime_BinaryLoading_EfkEfc("Runtime.BinaryLoading.EfkEfc", []()
										  { TestEfkEfcChunkBounds(); });
TestRegister Runtime_BinaryLoading_Gradient("Runtime.BinaryLoading.Gradient", []()
											{ TestGradientBounds(); });
TestRegister Runtime_BinaryLoading_Other("Runtime.BinaryLoading.Other", []()
										 { TestOtherBinaryBounds(); });
TestRegister Runtime_BinaryLoading_ValidMaterial("Runtime.BinaryLoading.ValidMaterial", []()
												 { TestValidMaterial(); });
TestRegister Runtime_BinaryLoading_ValidEffect("Runtime.BinaryLoading.ValidEffect", []()
											   { TestValidEffect(); });
} // namespace

#ifdef _WIN32

#ifdef __EFFEKSEER_BUILD_DX12__
#include "EffectPlatformDX12.h"
#endif

#include "EffectPlatformDX11.h"
#include "EffectPlatformDX9.h"
#include "EffectPlatformGL.h"
#elif defined(__APPLE__)
#include "EffectPlatformGL.h"
#else
#include "EffectPlatformGL.h"
#endif

#include "../Effekseer/Effekseer/SIMD/Effekseer.ConversionSIMD.h"
#include "../Effekseer/Effekseer/SIMD/Effekseer.Mat44fBlock4.h"
#include "../TestHelper.h"

void BasicRuntimeTestPlatform(EffectPlatform* platform, std::string baseResultPath, std::string suffix)
{
	EffectPlatformInitializingParameter param;
	platform->Initialize(param);

	auto single10Test = [&](const char16_t* name, const char* savename) -> void {
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/" + name + u".efk").c_str());

		for (size_t i = 0; i < 30; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str());
		platform->StopAllEffects();
	};

	auto single14Test = [&](const char16_t* name, const char* savename) -> void {
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/14/" + name + u".efk").c_str());

		for (size_t i = 0; i < 30; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str());
		platform->StopAllEffects();
	};

	auto single15Test = [&](const char16_t* name, const char* savename) -> void {
		srand(0);
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/15/" + name + u".efkefc").c_str());

		for (size_t i = 0; i < 30; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot((std::string(baseResultPath) + savename + suffix + ".png").c_str());
		platform->StopAllEffects();
	};

	single10Test(u"SimpleLaser", "SimpleLaser");
	single10Test(u"FCurve_Parameters1", "FCurve_Parameters1");
	single10Test(u"Ribbon_Parameters1", "Ribbon_Parameters1");
	single10Test(u"Ring_Parameters1", "Ring_Parameters1");
	single10Test(u"Track_Parameters1", "Track_Parameters1");
	single10Test(u"Sprite_Parameters1", "Sprite_Parameters1");
	single10Test(u"Distortions1", "Distortions1");

	single14Test(u"Model_Parameters1", "Model_Parameters1");
	single15Test(u"Material_Sampler1", "Material_Sampler1");
	single15Test(u"Material_Refraction", "Material_Refraction");
	single15Test(u"Material_WorldPositionOffset", "Material_WorldPositionOffset");
}

void BasicRuntimeDeviceLostTest()
{
#ifdef _WIN32
	auto platform = std::make_shared<EffectPlatformDX9>();

	srand(0);

	EffectPlatformInitializingParameter param;

	platform->Initialize(param);

	platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());

	for (size_t i = 0; i < 20; i++)
	{
		platform->Update();
	}
	platform->TakeScreenshot("0_Lost1.png");

	platform->SetFullscreen(true);

	for (size_t i = 0; i < 20; i++)
	{
		platform->Update();
	}
	platform->TakeScreenshot("0_Lost2.png");

	platform->SetFullscreen(false);

	for (size_t i = 0; i < 20; i++)
	{
		platform->Update();
	}
	platform->TakeScreenshot("0_Lost3.png");

	platform->Terminate();
#endif
}

void StartingFrameTest()
{
	srand(0);
	auto platform = std::make_shared<EffectPlatformGL>();

	EffectPlatformInitializingParameter param;

	platform->Initialize(param);

	platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str(), 30);

	for (size_t i = 0; i < 20; i++)
	{
		platform->Update();
	}
	platform->TakeScreenshot("StartingFrame_0.png");

	platform->Terminate();
}

void UpdateHandleTest()
{
	{
		srand(0);
		auto platform = std::make_shared<EffectPlatformGL>();

		EffectPlatformInitializingParameter param;

		platform->Initialize(param);

		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Ribbon_Parameters1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Parents1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Parents1.efk").c_str());

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot("UpdateHandle_0.png");

		platform->Terminate();
	}

	{
		srand(0);
		auto platform = std::make_shared<EffectPlatformGL>();

		EffectPlatformInitializingParameter param;
		param.IsUpdatedByHandle = true;

		platform->Initialize(param);

		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Ribbon_Parameters1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Parents1.efk").c_str());
		platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/Parents1.efk").c_str());

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot("UpdateHandle_1.png");

		platform->Terminate();
	}
}

void PlaybackSpeedTest() {
	{
		srand(0);
		auto platform = std::make_shared<EffectPlatformGL>();

		EffectPlatformInitializingParameter param;

		platform->Initialize(param);

		auto h = platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->GetManager()->SetSpeed(h, 0.5f);

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot("PlaybackSpeed_0.png");

		platform->Terminate();
	}

	{
		srand(0);
		auto platform = std::make_shared<EffectPlatformGL>();

		EffectPlatformInitializingParameter param;
		param.IsUpdatedByHandle = true;

		platform->Initialize(param);

		auto h = platform->Play((GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/10/SimpleLaser.efk").c_str());
		platform->GetManager()->SetSpeed(h, 0.5f);

		for (size_t i = 0; i < 20; i++)
		{
			platform->Update();
		}
		platform->TakeScreenshot("PlaybackSpeed_1.png");

		platform->Terminate();
	}
}

void BasicRuntimeTest()
{
#ifdef _WIN32
#ifdef __EFFEKSEER_BUILD_DX12__
	{
		auto platform = std::make_shared<EffectPlatformDX12>();
		BasicRuntimeTestPlatform(platform.get(), "", "_DX12");
		platform->Terminate();
	}
#endif

	{
		auto platform = std::make_shared<EffectPlatformDX9>();
		BasicRuntimeTestPlatform(platform.get(), "", "_DX9");
		platform->Terminate();
	}

	{
		auto platform = std::make_shared<EffectPlatformDX11>();
		BasicRuntimeTestPlatform(platform.get(), "", "_DX11");
		platform->Terminate();
	}

	{
		auto platform = std::make_shared<EffectPlatformGL>();
		BasicRuntimeTestPlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}

#elif defined(__APPLE__)
	{
		auto platform = std::make_shared<EffectPlatformGL>();
		BasicRuntimeTestPlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}
#else
	{
		auto platform = std::make_shared<EffectPlatformGL>();
		BasicRuntimeTestPlatform(platform.get(), "", "_GL");
		platform->Terminate();
	}
#endif
}

void CustomAllocatorTest()
{
	Effekseer::CustomVector<int> v;
	v.push_back(10);

	Effekseer::CustomList<int> l;
	l.push_back(10);

	Effekseer::CustomSet<int> s;
	s.insert(10);

	Effekseer::CustomMap<int, int> m;
	m[1] = 10;
}

void SIMDTest()
{
	// block mul
	{

		Effekseer::Matrix44 mat1[4];
		Effekseer::Matrix44 mat2[4];
		for (int i = 0; i < 4; i++)
		{
			mat1[i].Scaling(i * 1.0f, i * 2.0f, i * 3.0f);
			mat2[i].Translation(1.0f * i, i * 2.0f, i * i);
		}

		Effekseer::Mat44f smat1[4];
		Effekseer::Mat44f smat2[4];
		for (int i = 0; i < 4; i++)
		{
			smat1[i] = Effekseer::ToSIMD(mat1[i]);
			smat2[i] = Effekseer::ToSIMD(mat2[i]);
		}

		Effekseer::Mat44fBlock4 block, block1, block2;
		block1.Set(smat1[0], smat1[1], smat1[2], smat1[3]);
		block2.Set(smat2[0], smat2[1], smat2[2], smat2[3]);
		Effekseer::Mat44fBlock4::Mul(block, block1, block2);

		Effekseer::Mat44f sresult[4];
		block.Get(sresult[0], sresult[1], sresult[2], sresult[3]);

		Effekseer::Matrix44 result[4];
		for (int i = 0; i < 4; i++)
		{
			Effekseer::Matrix44::Mul(result[i], mat1[i], mat2[i]);
		}

		float diff = 0.0f;
		
		for (int k = 0; k < 4; k++)
		{
			for (int j = 0; j < 4; j++)
			{
				for (int i = 0; i < 4; i++)
				{
					diff += (result[k].Values[j][i] - Effekseer::ToStruct(sresult[k]).Values[j][i]);
				}
			}
		}

		assert(diff < 0.1f);
	}
}
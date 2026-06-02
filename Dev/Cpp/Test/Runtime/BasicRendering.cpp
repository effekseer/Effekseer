#ifdef _WIN32

#ifdef __EFFEKSEER_BUILD_DX12__
#include <Runtime/EffectPlatformDX12.h>
#endif

#include <Runtime/EffectPlatformDX11.h>
#include <Runtime/EffectPlatformDX9.h>
#elif defined(__APPLE__)
#include <Runtime/EffectPlatformMetal.h>
#else
#endif

#include <Runtime/EffectPlatformGL.h>

#ifdef __EFFEKSEER_BUILD_VULKAN__
#include <Runtime/EffectPlatformVulkan.h>
#endif

#ifdef __EFFEKSEER_BUILD_WEBGPU__
#include <Runtime/EffectPlatformWebGPU.h>
#endif

#include "BasicRendering.h"
#include "../TestHelper.h"
#include <iostream>
#include <utility>
#include <vector>

namespace
{

enum class BasicRenderingCaseRoot
{
	TestDataEffects,
	ResourceData00Basic,
	ResourceDataSamples,
};

enum class BasicRenderingCamera
{
	Default,
	Fireworks,
	SoftParticle,
};

enum class BasicRenderingPlayMode
{
	Single,
	PerformanceGrid,
};

struct BasicRenderingCase
{
	const char* Name = nullptr;
	BasicRenderingCaseRoot Root = BasicRenderingCaseRoot::TestDataEffects;
	const char16_t* Directory = nullptr;
	const char16_t* EffectName = nullptr;
	const char16_t* Extension = nullptr;
	const char* ScreenshotName = nullptr;
	int32_t FrameCount = 30;
	BasicRenderingCamera Camera = BasicRenderingCamera::Default;
	EffekseerRenderer::UVStyle BackgroundTextureUVStyle = EffekseerRenderer::UVStyle::Normal;
	bool OverridesBackgroundTextureUVStyle = false;
	bool UseGroundDepth = false;
	BasicRenderingPlayMode PlayMode = BasicRenderingPlayMode::Single;
};

const std::vector<BasicRenderingCase>& GetBasicRenderingCases()
{
	static const std::vector<BasicRenderingCase> cases = {
		{"Simple_Turbulence_Fireworks", BasicRenderingCaseRoot::ResourceData00Basic, nullptr, u"Simple_Turbulence_Fireworks", u".efkefc", "Simple_Turbulence_Fireworks", 180, BasicRenderingCamera::Fireworks},
		{"hit_hanmado_0409", BasicRenderingCaseRoot::ResourceDataSamples, u"03_Hanmado01/Effect", u"hit_hanmado_0409", u".efkefc", "hit_hanmado_0409", 30, BasicRenderingCamera::Default, EffekseerRenderer::UVStyle::Normal, false, false, BasicRenderingPlayMode::PerformanceGrid},

		{"SimpleLaser", BasicRenderingCaseRoot::TestDataEffects, u"10", u"SimpleLaser", u".efk", "SimpleLaser"},
		{"FCurve_Parameters1", BasicRenderingCaseRoot::TestDataEffects, u"10", u"FCurve_Parameters1", u".efk", "FCurve_Parameters1"},
		{"Ribbon_Parameters1", BasicRenderingCaseRoot::TestDataEffects, u"10", u"Ribbon_Parameters1", u".efk", "Ribbon_Parameters1"},
		{"Ring_Parameters1", BasicRenderingCaseRoot::TestDataEffects, u"10", u"Ring_Parameters1", u".efk", "Ring_Parameters1"},
		{"Track_Parameters1", BasicRenderingCaseRoot::TestDataEffects, u"10", u"Track_Parameters1", u".efk", "Track_Parameters1"},
		{"Sprite_Parameters1", BasicRenderingCaseRoot::TestDataEffects, u"10", u"Sprite_Parameters1", u".efk", "Sprite_Parameters1"},
		{"Distortions1", BasicRenderingCaseRoot::TestDataEffects, u"10", u"Distortions1", u".efk", "Distortions1"},

		{"Model_Parameters1", BasicRenderingCaseRoot::TestDataEffects, u"14", u"Model_Parameters1", u".efk", "Model_Parameters1"},

		{"Lighing_Parameters1", BasicRenderingCaseRoot::TestDataEffects, u"15", u"Lighing_Parameters1", u".efkefc", "Lighing_Parameters1"},
		{"DynamicParameter1", BasicRenderingCaseRoot::TestDataEffects, u"15", u"DynamicParameter1", u".efkefc", "DynamicParameter1"},
		{"Material_Sampler1", BasicRenderingCaseRoot::TestDataEffects, u"15", u"Material_Sampler1", u".efkefc", "Material_Sampler1"},
		{"Material_Refraction", BasicRenderingCaseRoot::TestDataEffects, u"15", u"Material_Refraction", u".efkefc", "Material_Refraction"},
		{"Material_WorldPositionOffset", BasicRenderingCaseRoot::TestDataEffects, u"15", u"Material_WorldPositionOffset", u".efkefc", "Material_WorldPositionOffset"},
		{"BasicRenderSettings_Blend", BasicRenderingCaseRoot::TestDataEffects, u"15", u"BasicRenderSettings_Blend", u".efkefc", "BasicRenderSettings_Blend"},
		{"BasicRenderSettings_Inherit_Color", BasicRenderingCaseRoot::TestDataEffects, u"15", u"BasicRenderSettings_Inherit_Color", u".efkefc", "BasicRenderSettings_Inherit_Color"},
		{"ForceFieldLocal_Turbulence1", BasicRenderingCaseRoot::TestDataEffects, u"15", u"ForceFieldLocal_Turbulence1", u".efkefc", "ForceFieldLocal_Turbulence1"},
		{"ForceFieldLocal_Old", BasicRenderingCaseRoot::TestDataEffects, u"15", u"ForceFieldLocal_Old", u".efkefc", "ForceFieldLocal_Old"},
		{"Material_FresnelRotatorPolarCoords", BasicRenderingCaseRoot::TestDataEffects, u"15", u"Material_FresnelRotatorPolarCoords", u".efkefc", "Material_FresnelRotatorPolarCoords"},
		{"Update_Easing", BasicRenderingCaseRoot::TestDataEffects, u"15", u"Update_Easing", u".efkefc", "Update_Easing"},
		{"Update_MultiModel", BasicRenderingCaseRoot::TestDataEffects, u"15", u"Update_MultiModel", u".efkefc", "Update_MultiModel"},
		{"Material_UV1", BasicRenderingCaseRoot::TestDataEffects, u"15", u"Material_UV1", u".efkefc", "Material_UV1"},
		{"Material_UV2", BasicRenderingCaseRoot::TestDataEffects, u"15", u"Material_UV2", u".efkefc", "Material_UV2"},
		{"Material_CustomData1", BasicRenderingCaseRoot::TestDataEffects, u"15", u"Material_CustomData1", u".efkefc", "Material_CustomData1"},
		{"Material_CustomDataMax", BasicRenderingCaseRoot::TestDataEffects, u"15", u"Material_CustomDataMax", u".efkefc", "Material_CustomDataMax"},
		{"SpawnMethodParameter1", BasicRenderingCaseRoot::TestDataEffects, u"15", u"SpawnMethodParameter1", u".efkefc", "SpawnMethodParameter1"},

		{"DrawWithoutInstancing", BasicRenderingCaseRoot::TestDataEffects, u"16", u"DrawWithoutInstancing", u".efkefc", "DrawWithoutInstancing"},
		{"AlphaBlendTexture01", BasicRenderingCaseRoot::TestDataEffects, u"16", u"AlphaBlendTexture01", u".efkefc", "AlphaBlendTexture01"},
		{"AlphaCutoffEdgeColor01", BasicRenderingCaseRoot::TestDataEffects, u"16", u"AlphaCutoffEdgeColor01", u".efkefc", "AlphaCutoffEdgeColor01"},
		{"BasicRenderSettings_Emissive", BasicRenderingCaseRoot::TestDataEffects, u"16", u"BasicRenderSettings_Emissive", u".efkefc", "BasicRenderSettings_Emissive"},
		{"Curve01", BasicRenderingCaseRoot::TestDataEffects, u"16", u"Curve01", u".efkefc", "Curve01"},
		{"EdgeFallOff01", BasicRenderingCaseRoot::TestDataEffects, u"16", u"EdgeFallOff01", u".efkefc", "EdgeFallOff01"},
		{"Flip01", BasicRenderingCaseRoot::TestDataEffects, u"16", u"Flip01", u".efkefc", "Flip01"},
		{"Flip_UV_01", BasicRenderingCaseRoot::TestDataEffects, u"16", u"Flip_UV_01", u".efkefc", "Flip_UV_01"},
		{"ForceFieldLocal02", BasicRenderingCaseRoot::TestDataEffects, u"16", u"ForceFieldLocal02", u".efkefc", "ForceFieldLocal02"},
		{"ForceFieldLocal03", BasicRenderingCaseRoot::TestDataEffects, u"16", u"ForceFieldLocal03", u".efkefc", "ForceFieldLocal03"},
		{"Material_EffectScale", BasicRenderingCaseRoot::TestDataEffects, u"16", u"Material_EffectScale", u".efkefc", "Material_EffectScale"},
		{"TGA01", BasicRenderingCaseRoot::TestDataEffects, u"16", u"TGA01", u".efkefc", "TGA01"},
		{"ProcedualModel01", BasicRenderingCaseRoot::TestDataEffects, u"16", u"ProcedualModel01", u".efkefc", "ProcedualModel01"},
		{"ProcedualModel02", BasicRenderingCaseRoot::TestDataEffects, u"16", u"ProcedualModel02", u".efkefc", "ProcedualModel02"},
		{"ProcedualModel03", BasicRenderingCaseRoot::TestDataEffects, u"16", u"ProcedualModel03", u".efkefc", "ProcedualModel03"},
		{"AlphaCutoffParameter01", BasicRenderingCaseRoot::TestDataEffects, u"16", u"AlphaCutoffParameter01", u".efkefc", "AlphaCutoffParameter01"},
		{"RotateScale01", BasicRenderingCaseRoot::TestDataEffects, u"16", u"RotateScale01", u".efkefc", "RotateScale01"},
		{"FollowParent01", BasicRenderingCaseRoot::TestDataEffects, u"16", u"FollowParent01", u".efkefc", "FollowParent01"},
		{"SoftParticle01_NotFlipped", BasicRenderingCaseRoot::TestDataEffects, u"16", u"SoftParticle01", u".efkefc", "SoftParticle01_NotFlipped", 30, BasicRenderingCamera::SoftParticle, EffekseerRenderer::UVStyle::Normal, false, true},
		{"SoftParticle01_Flipped", BasicRenderingCaseRoot::TestDataEffects, u"16", u"SoftParticle01", u".efkefc", "SoftParticle01_Flipped", 30, BasicRenderingCamera::SoftParticle, EffekseerRenderer::UVStyle::VerticalFlipped, true, true},

		{"Flip_UV_02", BasicRenderingCaseRoot::TestDataEffects, u"17", u"Flip_UV_02", u".efkefc", "Flip_UV_02", 57},
		{"Gradient1", BasicRenderingCaseRoot::TestDataEffects, u"17", u"Gradient1", u".efkefc", "Gradient1", 60},
		{"KillRules01", BasicRenderingCaseRoot::TestDataEffects, u"17", u"KillRules01", u".efkefc", "KillRules01", 60},
		{"Light1", BasicRenderingCaseRoot::TestDataEffects, u"17", u"Light1", u".efkefc", "Light1", 60},
		{"LocalTime", BasicRenderingCaseRoot::TestDataEffects, u"17", u"LocalTime", u".efkefc", "LocalTime", 60},
		{"Noise1", BasicRenderingCaseRoot::TestDataEffects, u"17", u"Noise1", u".efkefc", "Noise1", 60},

		{"Collisions", BasicRenderingCaseRoot::TestDataEffects, u"18", u"Collisions", u".efkefc", "Collisions"},
		{"GpuParticles_emit_mesh", BasicRenderingCaseRoot::TestDataEffects, u"18", u"GpuParticles_emit_mesh", u".efkefc", "GpuParticles_emit_mesh", 120},
		{"GpuParticles_sprite_simple", BasicRenderingCaseRoot::TestDataEffects, u"18", u"GpuParticles_sprite_simple", u".efkefc", "GpuParticles_sprite_simple", 120},
		{"GpuParticles_trails_simple", BasicRenderingCaseRoot::TestDataEffects, u"18", u"GpuParticles_trails_simple", u".efkefc", "GpuParticles_trails_simple", 120},
		{"Materials1", BasicRenderingCaseRoot::TestDataEffects, u"18", u"Materials1", u".efkefc", "Materials1"},
		{"Materials2", BasicRenderingCaseRoot::TestDataEffects, u"18", u"Materials2", u".efkefc", "Materials2"},
	};
	return cases;
}

const BasicRenderingCase* FindBasicRenderingCase(std::string_view name)
{
	for (const auto& testCase : GetBasicRenderingCases())
	{
		if (name == testCase.Name)
		{
			return &testCase;
		}
	}
	return nullptr;
}

std::string MakeBasicRenderingSuffix(const EffectPlatformInitializingParameter& param, std::string suffix)
{
	if (param.CoordinateSyatem == Effekseer::CoordinateSystem::LH)
	{
		suffix += ".LH";
	}
	return suffix;
}

std::u16string MakeBasicRenderingEffectPath(const BasicRenderingCase& testCase)
{
	const auto directory = GetDirectoryPathAsU16(__FILE__);
	if (testCase.Root == BasicRenderingCaseRoot::ResourceData00Basic)
	{
		return directory + u"../../../../ResourceData/samples/00_Basic/" + testCase.EffectName + testCase.Extension;
	}
	if (testCase.Root == BasicRenderingCaseRoot::ResourceDataSamples)
	{
		return directory + u"../../../../ResourceData/samples/" + testCase.Directory + u"/" + testCase.EffectName + testCase.Extension;
	}

	return directory + u"../../../../TestData/Effects/" + testCase.Directory + u"/" + testCase.EffectName + testCase.Extension;
}

void PlayBasicRenderingEffect(EffectPlatform* platform, const BasicRenderingCase& testCase, const std::u16string& path)
{
	if (testCase.PlayMode == BasicRenderingPlayMode::PerformanceGrid)
	{
		constexpr float gridMin = -2.0f;
		constexpr float gridMax = 2.0f;
		constexpr float gridStep = 2.0f;
		constexpr float eps = 0.0001f;

		for (float y = gridMin; y <= gridMax + eps; y += gridStep)
		{
			for (float x = gridMin; x <= gridMax + eps; x += gridStep)
			{
				platform->Play(path.c_str(), {x, y, 0.0f});
			}
		}
		return;
	}

	platform->Play(path.c_str());
}

void SetBasicRenderingCamera(const EffectPlatformInitializingParameter& param, EffectPlatform* platform, BasicRenderingCamera camera)
{
	Effekseer::Matrix44 mat;
	switch (camera)
	{
	case BasicRenderingCamera::Fireworks:
		if (param.CoordinateSyatem == Effekseer::CoordinateSystem::RH)
		{
			mat.LookAtRH({0, 20, 20}, {0, 20, 0}, {0, 1, 0});
		}
		else
		{
			mat.LookAtLH({0, 20, -20}, {0, 20, 0}, {0, 1, 0});
		}
		break;
	case BasicRenderingCamera::SoftParticle:
		if (param.CoordinateSyatem == Effekseer::CoordinateSystem::RH)
		{
			mat.LookAtRH({0, 3, 10}, {0, 0, 0}, {0, 1, 0});
		}
		else
		{
			mat.LookAtLH({0, 3, -10}, {0, 0, 0}, {0, 1, 0});
		}
		break;
	default:
		return;
	}

	platform->GetRenderer()->SetCameraMatrix(mat);
}

void RunBasicRenderingCase(
	const EffectPlatformInitializingParameter& param,
	EffectPlatform* platform,
	const std::string& baseResultPath,
	const std::string& suffix,
	const BasicRenderingCase& testCase,
	const std::string& screenshotPrefix)
{
	const auto renderer = platform->GetRenderer();
	const auto cameraMat = renderer->GetCameraMatrix();
	const auto backgroundTextureUVStyle = renderer->GetBackgroundTextureUVStyle();
	auto background = renderer->GetBackground();
	::Effekseer::Backend::TextureRef depthTexture;
	::EffekseerRenderer::DepthReconstructionParameter depthReconstructionParam{};
	renderer->GetDepth(depthTexture, depthReconstructionParam);
	const auto changesCamera = testCase.Camera != BasicRenderingCamera::Default;

	if (changesCamera)
	{
		SetBasicRenderingCamera(param, platform, testCase.Camera);
	}
	if (testCase.OverridesBackgroundTextureUVStyle)
	{
		renderer->SetBackgroundTextureUVStyle(testCase.BackgroundTextureUVStyle);
	}
	if (testCase.UseGroundDepth)
	{
		platform->GenerateGroundDepth();
	}

	srand(0);
	const auto path = MakeBasicRenderingEffectPath(testCase);
	PlayBasicRenderingEffect(platform, testCase, path);

	for (int32_t i = 0; i < testCase.FrameCount; i++)
	{
		platform->Update();
	}

	const auto screenshotPath = baseResultPath + screenshotPrefix + testCase.ScreenshotName + suffix + ".png";
	platform->TakeScreenshot(screenshotPath.c_str());
	platform->StopAllEffects();
	platform->ClearLoadedEffects();

	if (changesCamera)
	{
		renderer->SetCameraMatrix(cameraMat);
	}
	if (testCase.OverridesBackgroundTextureUVStyle)
	{
		renderer->SetBackgroundTextureUVStyle(backgroundTextureUVStyle);
	}
	if (testCase.UseGroundDepth)
	{
		platform->ResetBackgroundPattern();
	}
	renderer->SetBackground(background);
	renderer->SetDepth(depthTexture, depthReconstructionParam);
}

template <class T>
void RegisterBasicRuntimeTestPlatformCasesFor(const char* platformName, const char* suffix)
{
	RegisterBasicRuntimeTestPlatformCases(
		platformName,
		suffix,
		[]() -> std::shared_ptr<EffectPlatform>
		{
			return std::make_shared<T>();
		});
}

struct BasicRenderingCaseTestRegistration
{
	BasicRenderingCaseTestRegistration()
	{
#ifdef _WIN32
		RegisterBasicRuntimeTestPlatformCasesFor<EffectPlatformDX11>("DX11", "_DX11");
#endif

#if !defined(__FROM_CI__)
#ifdef __EFFEKSEER_BUILD_VULKAN__
		RegisterBasicRuntimeTestPlatformCasesFor<EffectPlatformVulkan>("Vulkan", "_Vulkan");
#endif

#ifdef _WIN32
#ifdef __EFFEKSEER_BUILD_DX12__
		RegisterBasicRuntimeTestPlatformCasesFor<EffectPlatformDX12>("DX12", "_DX12");
#endif
		RegisterBasicRuntimeTestPlatformCasesFor<EffectPlatformDX9>("DX9", "_DX9");
		RegisterBasicRuntimeTestPlatformCasesFor<EffectPlatformGL>("GL", "_GL");
#elif defined(__APPLE__)
		RegisterBasicRuntimeTestPlatformCasesFor<EffectPlatformMetal>("Metal", "_Metal");
		RegisterBasicRuntimeTestPlatformCasesFor<EffectPlatformGL>("GL", "_GL");
#else
#ifndef __EFFEKSEER_BUILD_VERSION16__
		RegisterBasicRuntimeTestPlatformCasesFor<EffectPlatformGL>("GL", "_GL");
#endif
#endif
#endif
	}
};

BasicRenderingCaseTestRegistration basicRenderingCaseTestRegistration;

} // namespace

void BasicRuntimeTestPlatform(EffectPlatformInitializingParameter param, EffectPlatform* platform, std::string baseResultPath, std::string suffix)
{
	platform->Initialize(param);
	suffix = MakeBasicRenderingSuffix(param, std::move(suffix));

	for (const auto& testCase : GetBasicRenderingCases())
	{
		RunBasicRenderingCase(param, platform, baseResultPath, suffix, testCase, "");
	}
}

void BasicRuntimeTestPlatformCase(
	EffectPlatformInitializingParameter param,
	EffectPlatform* platform,
	std::string baseResultPath,
	std::string suffix,
	std::string_view caseName)
{
	BasicRuntimeTestPlatformCases(param, platform, std::move(baseResultPath), std::move(suffix), {caseName});
}

void BasicRuntimeTestPlatformCases(
	EffectPlatformInitializingParameter param,
	EffectPlatform* platform,
	std::string baseResultPath,
	std::string suffix,
	std::initializer_list<std::string_view> caseNames,
	std::string screenshotPrefix)
{
	platform->Initialize(param);
	suffix = MakeBasicRenderingSuffix(param, std::move(suffix));

	for (const auto caseName : caseNames)
	{
		const auto testCase = FindBasicRenderingCase(caseName);
		EXPECT_TRUE(testCase != nullptr);
		RunBasicRenderingCase(param, platform, baseResultPath, suffix, *testCase, screenshotPrefix);
	}
}

void RegisterBasicRuntimeTestPlatformCases(
	const char* platformName,
	const char* suffix,
	std::function<std::shared_ptr<EffectPlatform>()> createPlatform)
{
	const auto allCasesTestName = std::string("Runtime.BasicRendering.") + platformName;
	TestHelper::RegisterTest(
		allCasesTestName.c_str(),
		[suffix = std::string(suffix), createPlatform]() -> void
		{
			EffectPlatformInitializingParameter param;
			auto platform = createPlatform();
			BasicRuntimeTestPlatform(param, platform.get(), "", suffix);
			platform->Terminate();
		},
		TestExecutionMode::FilterOnly);

	for (const auto& testCase : GetBasicRenderingCases())
	{
		const auto testName = std::string("Runtime.BasicRendering.") + platformName + "." + testCase.Name;
		TestHelper::RegisterTest(
			testName.c_str(),
			[caseName = std::string(testCase.Name), suffix = std::string(suffix), createPlatform]() -> void
			{
				EffectPlatformInitializingParameter param;
				auto platform = createPlatform();
				BasicRuntimeTestPlatformCase(param, platform.get(), "", suffix, caseName);
				platform->Terminate();
			},
			TestExecutionMode::FilterOnly);
	}
}

template <class T>
void RunBasicRuntimeTestOnPlatform(const EffectPlatformInitializingParameter& param, const char* suffix)
{
	auto platform = std::make_shared<T>();
	BasicRuntimeTestPlatform(param, platform.get(), "", suffix);
	platform->Terminate();
}

void BasicRuntimeTest()
{
	EffectPlatformInitializingParameter param;
	// param.CoordinateSyatem = Effekseer::CoordinateSystem::LH;

#ifdef _WIN32
	{
		RunBasicRuntimeTestOnPlatform<EffectPlatformDX11>(param, "_DX11");
	}
#endif

#if !defined(__FROM_CI__)
#ifdef __EFFEKSEER_BUILD_VULKAN__
	{
		RunBasicRuntimeTestOnPlatform<EffectPlatformVulkan>(param, "_Vulkan");
	}
#endif

#ifdef __EFFEKSEER_BUILD_WEBGPU__
	{
		RunBasicRuntimeTestOnPlatform<EffectPlatformWebGPU>(param, "_WebGPU");
	}
#endif

#ifdef _WIN32
	{

#ifdef __EFFEKSEER_BUILD_DX12__
		{
			RunBasicRuntimeTestOnPlatform<EffectPlatformDX12>(param, "_DX12");
		}
#endif

		{
			RunBasicRuntimeTestOnPlatform<EffectPlatformDX9>(param, "_DX9");
		}

		{
			RunBasicRuntimeTestOnPlatform<EffectPlatformGL>(param, "_GL");
		}
	}

#elif defined(__APPLE__)

	{
		RunBasicRuntimeTestOnPlatform<EffectPlatformMetal>(param, "_Metal");
	}

	{
		RunBasicRuntimeTestOnPlatform<EffectPlatformGL>(param, "_GL");
	}

#else
#ifndef __EFFEKSEER_BUILD_VERSION16__
	{
		RunBasicRuntimeTestOnPlatform<EffectPlatformGL>(param, "_GL");
	}
#endif
#endif
#endif
}

TestRegister Runtime_BasicRuntimeTest("Runtime.BasicRuntimeTest", []() -> void
									  { BasicRuntimeTest(); });

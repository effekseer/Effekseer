#pragma once

// Keep backend availability in one place. Tests select all renderers or only
// LLGI renderers when they need GPU readback/explicit command lists.
#include <Runtime/EffectPlatform.h>
#ifndef EFFEKSEER_TEST_LLGI_ONLY
#include <Runtime/EffectPlatformGL.h>
#ifdef _WIN32
#include <Runtime/EffectPlatformDX11.h>
#include <Runtime/EffectPlatformDX9.h>
#endif
#endif
#ifdef __EFFEKSEER_BUILD_DX12__
#include <Runtime/EffectPlatformDX12.h>
#endif
#ifdef __EFFEKSEER_BUILD_VULKAN__
#include <Runtime/EffectPlatformVulkan.h>
#endif
#ifdef __EFFEKSEER_BUILD_METAL__
#include <Runtime/EffectPlatformMetal.h>
#endif
#ifdef __EFFEKSEER_BUILD_WEBGPU__
#include <Runtime/EffectPlatformWebGPU.h>
#endif

template <class T>
struct TestPlatformType
{
	using Type = T;
};

template <class Register>
void ForEachLLGITestPlatform(Register registerTests)
{
#ifdef __EFFEKSEER_BUILD_DX12__
	registerTests(TestPlatformType<EffectPlatformDX12>{}, "DX12");
#endif
#ifdef __EFFEKSEER_BUILD_VULKAN__
	registerTests(TestPlatformType<EffectPlatformVulkan>{}, "Vulkan");
#endif
#ifdef __EFFEKSEER_BUILD_METAL__
	registerTests(TestPlatformType<EffectPlatformMetal>{}, "Metal");
#endif
#ifdef __EFFEKSEER_BUILD_WEBGPU__
	registerTests(TestPlatformType<EffectPlatformWebGPU>{}, "WebGPU");
#endif
}

template <class Register>
void ForEachTestPlatform(Register registerTests)
{
#ifndef EFFEKSEER_TEST_LLGI_ONLY
	registerTests(TestPlatformType<EffectPlatformGL>{}, "GL");
#ifdef _WIN32
	registerTests(TestPlatformType<EffectPlatformDX11>{}, "DX11");
	registerTests(TestPlatformType<EffectPlatformDX9>{}, "DX9");
#endif
#endif
	ForEachLLGITestPlatform(registerTests);
}

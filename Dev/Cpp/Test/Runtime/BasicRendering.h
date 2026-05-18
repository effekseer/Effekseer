#pragma once

#include <Runtime/EffectPlatform.h>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>

void BasicRuntimeTestPlatform(EffectPlatformInitializingParameter param, EffectPlatform* platform, std::string baseResultPath, std::string suffix);
void BasicRuntimeTestPlatformCase(
	EffectPlatformInitializingParameter param,
	EffectPlatform* platform,
	std::string baseResultPath,
	std::string suffix,
	std::string_view caseName);
void BasicRuntimeTestPlatformCases(
	EffectPlatformInitializingParameter param,
	EffectPlatform* platform,
	std::string baseResultPath,
	std::string suffix,
	std::initializer_list<std::string_view> caseNames,
	std::string screenshotPrefix = "");
void RegisterBasicRuntimeTestPlatformCases(
	const char* platformName,
	const char* suffix,
	std::function<std::shared_ptr<EffectPlatform>()> createPlatform);

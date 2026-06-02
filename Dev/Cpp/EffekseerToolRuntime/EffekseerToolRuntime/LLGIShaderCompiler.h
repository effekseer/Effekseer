#pragma once

#include <Effekseer.h>
#include <LLGI.Base.h>

#include <string>
#include <vector>

namespace Effekseer
{
namespace ToolRuntime
{

struct LLGICompiledShader
{
	std::vector<std::vector<uint8_t>> Binary;

	bool IsEmpty() const;

	std::vector<LLGI::DataStructure> GetDataStructures() const;

	std::vector<uint8_t> Serialize() const;
};

struct LLGIShaderCompileInput
{
	const char* Name = nullptr;
	const char* Code = nullptr;
	LLGI::ShaderStageType Stage = LLGI::ShaderStageType::Vertex;
};

bool TryGetLLGIDeviceType(
	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice,
	LLGI::DeviceType& deviceType);

bool CompileLLGIShaders(
	LLGI::DeviceType deviceType,
	const LLGIShaderCompileInput* inputs,
	int32_t inputCount,
	LLGICompiledShader* outputs,
	std::string& message);

LLGI::Shader* CreateLLGIShaderFromCode(
	LLGI::Graphics* graphics,
	LLGI::DeviceType deviceType,
	const char* code,
	LLGI::ShaderStageType stage,
	std::string& message,
	const char* name = nullptr);

Effekseer::Backend::ShaderRef CreateBackendShaderFromLLGICode(
	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice,
	LLGI::DeviceType deviceType,
	const char* vertexCode,
	const char* pixelCode,
	std::string& message,
	const char* name = nullptr);

} // namespace ToolRuntime
} // namespace Effekseer

#include "LLGIShaderCompiler.h"

#include <EffekseerRendererLLGI/GraphicsDevice.h>
#include <LLGI.Compiler.h>
#include <LLGI.Graphics.h>
#include <LLGI.Shader.h>

#ifdef __EFFEKSEER_BUILD_DX12__
#include <DX12/LLGI.CompilerDX12.h>
#endif
#ifdef __EFFEKSEER_BUILD_VULKAN__
#include <Vulkan/LLGI.CompilerVulkan.h>
#endif
#ifdef __EFFEKSEER_BUILD_WEBGPU__
#include <WebGPU/LLGI.CompilerWebGPU.h>
#endif
#ifdef __APPLE__
#include <Metal/LLGI.CompilerMetal.h>
#endif

#include <utility>

namespace
{

LLGI::Compiler* CreateLLGICompiler(LLGI::DeviceType deviceType)
{
	switch (deviceType)
	{
	case LLGI::DeviceType::DirectX12:
#ifdef __EFFEKSEER_BUILD_DX12__
		return new LLGI::CompilerDX12();
#else
		return nullptr;
#endif
	case LLGI::DeviceType::Vulkan:
#ifdef __EFFEKSEER_BUILD_VULKAN__
		return new LLGI::CompilerVulkan();
#else
		return nullptr;
#endif
	case LLGI::DeviceType::Metal:
#ifdef __APPLE__
		return new LLGI::CompilerMetal();
#else
		return nullptr;
#endif
	case LLGI::DeviceType::WebGPU:
#ifdef __EFFEKSEER_BUILD_WEBGPU__
		return new LLGI::CompilerWebGPU();
#else
		return nullptr;
#endif
	default:
		return nullptr;
	}
}

const char* GetDefaultShaderName(LLGI::ShaderStageType stage)
{
	switch (stage)
	{
	case LLGI::ShaderStageType::Vertex:
		return "VS";
	case LLGI::ShaderStageType::Pixel:
		return "PS";
	case LLGI::ShaderStageType::Compute:
		return "CS";
	default:
		return "Shader";
	}
}

void AppendMessage(std::string& message, const std::string& line)
{
	if (!message.empty())
	{
		message += "\n";
	}
	message += line;
}

const char* GetShaderName(const Effekseer::ToolRuntime::LLGIShaderCompileInput& input)
{
	return input.Name != nullptr ? input.Name : GetDefaultShaderName(input.Stage);
}

} // namespace

namespace Effekseer
{
namespace ToolRuntime
{

bool TryGetLLGIDeviceType(
	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice,
	LLGI::DeviceType& deviceType)
{
	if (graphicsDevice == nullptr)
	{
		return false;
	}

	const auto deviceName = graphicsDevice->GetDeviceName();
	if (deviceName != "DirectX12" && deviceName != "Vulkan" && deviceName != "Metal" && deviceName != "WebGPU" && deviceName != "LLGI")
	{
		return false;
	}

	auto llgiGraphicsDevice = graphicsDevice.DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();
	if (llgiGraphicsDevice == nullptr)
	{
		return false;
	}

	deviceType = llgiGraphicsDevice->GetDeviceType();
	return true;
}

bool LLGICompiledShader::IsEmpty() const
{
	return Binary.empty();
}

std::vector<LLGI::DataStructure> LLGICompiledShader::GetDataStructures() const
{
	std::vector<LLGI::DataStructure> data;
	data.reserve(Binary.size());
	for (const auto& binary : Binary)
	{
		LLGI::DataStructure element;
		element.Data = binary.data();
		element.Size = static_cast<int32_t>(binary.size());
		data.push_back(element);
	}
	return data;
}

std::vector<uint8_t> LLGICompiledShader::Serialize() const
{
	return EffekseerRendererLLGI::Backend::Serialize(GetDataStructures());
}

bool CompileLLGIShaders(
	LLGI::DeviceType deviceType,
	const LLGIShaderCompileInput* inputs,
	int32_t inputCount,
	LLGICompiledShader* outputs,
	std::string& message)
{
	message.clear();

	if (inputs == nullptr || outputs == nullptr || inputCount <= 0)
	{
		message = "Invalid LLGI shader compile input.";
		return false;
	}

	auto compiler = CreateLLGICompiler(deviceType);
	if (compiler == nullptr)
	{
		message = "Failed to create an LLGI shader compiler.";
		return false;
	}

	compiler->Initialize();

	std::vector<LLGI::CompilerResult> results(static_cast<size_t>(inputCount));
	for (int32_t i = 0; i < inputCount; i++)
	{
		if (inputs[i].Code == nullptr)
		{
			AppendMessage(message, std::string(GetShaderName(inputs[i])) + ": shader code is null.");
			continue;
		}

		compiler->Compile(results[i], inputs[i].Code, inputs[i].Stage);
	}

	compiler->Release();

	for (int32_t i = 0; i < inputCount; i++)
	{
		if (!results[i].Message.empty())
		{
			AppendMessage(message, std::string(GetShaderName(inputs[i])) + ": " + results[i].Message);
		}
	}

	for (int32_t i = 0; i < inputCount; i++)
	{
		if (inputs[i].Code != nullptr && results[i].Message.empty() && results[i].Binary.empty())
		{
			AppendMessage(message, std::string(GetShaderName(inputs[i])) + ": shader binary is empty.");
		}
	}

	if (!message.empty())
	{
		return false;
	}

	for (int32_t i = 0; i < inputCount; i++)
	{
		outputs[i].Binary = std::move(results[i].Binary);
	}

	return true;
}

LLGI::Shader* CreateLLGIShaderFromCode(
	LLGI::Graphics* graphics,
	LLGI::DeviceType deviceType,
	const char* code,
	LLGI::ShaderStageType stage,
	std::string& message,
	const char* name)
{
	if (graphics == nullptr)
	{
		message = "LLGI graphics is null.";
		return nullptr;
	}

	LLGIShaderCompileInput input;
	input.Name = name;
	input.Code = code;
	input.Stage = stage;

	LLGICompiledShader compiled;
	if (!CompileLLGIShaders(deviceType, &input, 1, &compiled, message))
	{
		return nullptr;
	}

	auto data = compiled.GetDataStructures();
	return graphics->CreateShader(data.data(), static_cast<int32_t>(data.size()));
}

Effekseer::Backend::ShaderRef CreateBackendShaderFromLLGICode(
	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice,
	LLGI::DeviceType deviceType,
	const char* vertexCode,
	const char* pixelCode,
	std::string& message,
	const char* name)
{
	if (graphicsDevice == nullptr)
	{
		message = "GraphicsDevice is null.";
		return nullptr;
	}

	LLGIShaderCompileInput inputs[2];
	inputs[0].Name = "VS";
	inputs[0].Code = vertexCode;
	inputs[0].Stage = LLGI::ShaderStageType::Vertex;
	inputs[1].Name = "PS";
	inputs[1].Code = pixelCode;
	inputs[1].Stage = LLGI::ShaderStageType::Pixel;

	LLGICompiledShader compiled[2];
	if (!CompileLLGIShaders(deviceType, inputs, 2, compiled, message))
	{
		if (name != nullptr && !message.empty())
		{
			message = std::string(name) + ":\n" + message;
		}
		return nullptr;
	}

	const auto vertexBinary = compiled[0].Serialize();
	const auto pixelBinary = compiled[1].Serialize();
	return graphicsDevice->CreateShaderFromBinary(
		vertexBinary.data(),
		static_cast<int32_t>(vertexBinary.size()),
		pixelBinary.data(),
		static_cast<int32_t>(pixelBinary.size()));
}

} // namespace ToolRuntime
} // namespace Effekseer

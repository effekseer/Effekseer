#include "../TestHelper.h"
#include <EffekseerRendererDX11.h>
#include <EffekseerRendererDX11/GraphicsDevice.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <array>
#include <cstring>

namespace
{
void UniformBufferUpdatesDX11()
{
	using Microsoft::WRL::ComPtr;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	EXPECT_TRUE(SUCCEEDED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0,
		nullptr, 0, D3D11_SDK_VERSION, &device, nullptr, &context)));
	auto graphics = EffekseerRendererDX11::CreateGraphicsDevice(device.Get(), context.Get());
	const char* source =
		"cbuffer C : register(b0) { uint index; uint value; uint2 padding; };"
		"RWStructuredBuffer<uint> output : register(u0);"
		"[numthreads(1,1,1)] void main() { output[index] = value; }";
	ComPtr<ID3DBlob> binary, errors;
	EXPECT_TRUE(SUCCEEDED(D3DCompile(source, strlen(source), nullptr, nullptr, nullptr,
		"main", "cs_5_0", 0, 0, &binary, &errors)));
	auto shader = graphics->CreateComputeShader(binary->GetBufferPointer(), static_cast<int32_t>(binary->GetBufferSize()));
	Effekseer::Backend::PipelineStateParameter parameters;
	parameters.ShaderPtr = shader;
	auto pipeline = graphics->CreatePipelineState(parameters);
	std::array<uint32_t, 4> values{};
	auto uniform = graphics->CreateUniformBuffer(sizeof(values), values.data());
	constexpr uint32_t Steps = 36;
	auto output = graphics->CreateStorageBuffer(Steps, sizeof(uint32_t), nullptr,
		Effekseer::Backend::StorageBufferUsage::ReadWrite);
	Effekseer::Backend::DispatchParameter dispatch;
	dispatch.PipelineStatePtr = pipeline;
	dispatch.UniformBufferPtrs[0] = uniform;
	dispatch.SetStorageBuffer(0, output, Effekseer::Backend::StorageBufferAccess::ReadWrite);
	dispatch.GroupCount = {1, 1, 1};
	dispatch.ThreadCount = {1, 1, 1};
	for (uint32_t step = 0; step < Steps; step++)
	{
		values[0] = step;
		values[1] = step + 1;
		graphics->UpdateUniformBuffer(uniform, sizeof(values), 0, values.data());
		graphics->Dispatch(dispatch);
	}
	values.fill(0);
	graphics->UpdateUniformBuffer(uniform, sizeof(values), 0, values.data());
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = Steps * sizeof(uint32_t);
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	ComPtr<ID3D11Buffer> readback;
	EXPECT_TRUE(SUCCEEDED(device->CreateBuffer(&desc, nullptr, &readback)));
	context->CopyResource(readback.Get(), output.DownCast<EffekseerRendererDX11::Backend::StorageBuffer>()->GetBuffer());
	D3D11_MAPPED_SUBRESOURCE mapped{};
	EXPECT_TRUE(SUCCEEDED(context->Map(readback.Get(), 0, D3D11_MAP_READ, 0, &mapped)));
	auto actual = static_cast<const uint32_t*>(mapped.pData);
	for (uint32_t step = 0; step < Steps; step++)
	{
		EXPECT_TRUE(actual[step] == step + 1);
	}
	context->Unmap(readback.Get(), 0);
}
TestRegister registerUniformBufferUpdatesDX11("Backend.UniformBufferUpdatesDX11", UniformBufferUpdatesDX11);
} // namespace

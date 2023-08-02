#ifndef __EFFEKSEERRENDERER_GRAPHICS_DEVICE_CPU_H__
#define __EFFEKSEERRENDERER_GRAPHICS_DEVICE_CPU_H__

#include <Effekseer.h>

namespace EffekseerRendererCPU
{
namespace Backend
{

class VertexBuffer
	: public Effekseer::Backend::VertexBuffer
{
private:
	std::vector<uint8_t> buffer_;

public:
	VertexBuffer(int32_t size, const void* initialData, bool isDynamic);
	~VertexBuffer() override = default;
	void UpdateData(const void* src, int32_t size, int32_t offset) override;

	const std::vector<uint8_t>& GetBuffer() const
	{
		return buffer_;
	}
};

class IndexBuffer
	: public Effekseer::Backend::IndexBuffer
{
private:
	std::vector<uint8_t> buffer_;

public:
	IndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType strideType);
	~IndexBuffer() override = default;
	void UpdateData(const void* src, int32_t size, int32_t offset) override;
};

class GraphicsDevice : public Effekseer::Backend::GraphicsDevice
{
public:
	GraphicsDevice() = default;
	~GraphicsDevice() override = default;

	Effekseer::Backend::VertexBufferRef CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic) override;

	Effekseer::Backend::IndexBufferRef CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride) override;
};

} // namespace Backend
} // namespace EffekseerRendererCPU

#endif
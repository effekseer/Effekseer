
#ifndef __EFFEKSEER_GRAPHICS_DEVICE_H__
#define __EFFEKSEER_GRAPHICS_DEVICE_H__

#include "../Effekseer.Base.Pre.h"
#include <array>
#include <stdint.h>
#include <string>

namespace Effekseer
{
namespace Backend
{

class VertexBuffer
	: public ReferenceObject
{
public:
	VertexBuffer() = default;
	virtual ~VertexBuffer() = default;
};

class IndexBuffer
	: public ReferenceObject
{
public:
	IndexBuffer() = default;
	virtual ~IndexBuffer() = default;
};

class VertexLayout
	: public ReferenceObject
{
public:
	VertexLayout() = default;
	virtual ~VertexLayout() = default;
};

class UniformBuffer
	: public ReferenceObject
{
public:
	UniformBuffer() = default;
	virtual ~UniformBuffer() = default;
};

class PipelineState
	: public ReferenceObject
{
public:
	PipelineState() = default;
	virtual ~PipelineState() = default;
};

class Texture
	: public ReferenceObject
{
public:
	Texture() = default;
	virtual ~Texture() = default;

	/**
	 * TODO : Implement GetType, GetFormat, GetSize
	*/
};

class Shader
	: public ReferenceObject
{
public:
	Shader() = default;
	virtual ~Shader() = default;
};

class ComputeBuffer
	: public ReferenceObject
{
public:
	ComputeBuffer() = default;
	virtual ~ComputeBuffer() = default;
};

enum class IndexBufferStrideType
{
	Stride2,
	Stride4,
};

enum class TextureWrapType
{
	Clamp,
	Repeat,
};

enum class TextureSamplingType
{
	Linear,
	Nearest,
};

struct DrawParameter
{
public:
	VertexBuffer* VertexBufferPtr = nullptr;
	IndexBuffer* IndexBufferPtr = nullptr;
	PipelineState* PipelineStatePtr = nullptr;

	std::array<Texture*, 8> TexturePtrs;
	std::array<TextureWrapType, 8> TextureWrapTypes;
	std::array<TextureSamplingType, 8> TextureSamplingTypes;

	int32_t PrimitiveCount = 0;
	int32_t InstanceCount = 0;
};

enum class VertexLayoutFormat
{
	R32_FLOAT,
	R32G32_FLOAT,
	R32G32B32_FLOAT,
	R32G32B32A32_FLOAT,
	R8G8B8A8_UNORM,
	R8G8B8A8_UINT,
};

struct VertexLayoutElement
{
	VertexLayoutFormat Format;

	//! only for DirectX
	std::string SemanticName;

	//! only for DirectX
	int32_t SemanticIndex = 0;
};

enum class TopologyType
{
	Triangle,
	Line,
	Point,
};

enum class TextureFormatType
{
	R8G8B8A8_UNORM,
	B8G8R8A8_UNORM,
	R8_UNORM,
	R16G16_FLOAT,
	R16G16B16A16_FLOAT,
	R32G32B32A32_FLOAT,
	BC1,
	BC2,
	BC3,
	R8G8B8A8_UNORM_SRGB,
	B8G8R8A8_UNORM_SRGB,
	BC1_SRGB,
	BC2_SRGB,
	BC3_SRGB,
	D32,
	D24S8,
	D32S8,
	Unknown,
};

enum class CullingMode
{
	Clockwise,
	CounterClockwise,
	DoubleSide,
};

enum class BlendEquationType
{
	Add,
	Sub,
	ReverseSub,
	Min,
	Max,
};

enum class BlendFuncType
{
	Zero,
	One,
	SrcColor,
	OneMinusSrcColor,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstAlpha,
	OneMinusDstAlpha,
	DstColor,
	OneMinusDstColor,
};

enum class DepthFuncType
{
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always,
};

enum class CompareFuncType
{
	Never,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always,
};

struct PipelineStateParameter
{
	TopologyType Topology = TopologyType::Triangle;

	VertexLayout* VertexLayoutPtr = nullptr;
};

struct TextureParameter
{
	std::array<float, 2> Size;
};

struct RenderTextureParameter
{
	std::array<float, 2> Size;
};

struct DepthTextureParameter
{
	std::array<float, 2> Size;
};

class GraphicsDevice
	: public ReferenceObject
{
public:
	GraphicsDevice() = default;
	virtual ~GraphicsDevice() = default;

	/**
		@brief	Create VertexBuffer
		@param	size	the size of buffer
		@param	initialData	the initial data of buffer. If it is null, not initialized.
		@param	isDynamic	whether is the buffer dynamic? (for DirectX9, 11 or OpenGL)
		@return	VertexBuffer
	*/
	virtual VertexBuffer* CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic)
	{
		return nullptr;
	}

	/**
		@brief	Create IndexBuffer
		@param	elementCount	the number of element
		@param	initialData	the initial data of buffer. If it is null, not initialized.
		@param	stride	stride type
		@return	IndexBuffer
	*/
	virtual IndexBuffer* CreateIndexBuffer(int32_t elementCount, const void* initialData, IndexBufferStrideType stride)
	{
		return nullptr;
	}

	/**
		@brief	Update content of a vertex buffer
		@param	buffer	buffer
		@param	size	the size of updated buffer
		@param	offset	the offset of updated buffer
		@param	data	updating data
		@return	Succeeded in updating?
	*/
	virtual bool UpdateVertexBuffer(VertexBuffer* buffer, int32_t size, int32_t offset, const void* data)
	{
		return false;
	}

	/**
		@brief	Update content of a vertex buffer
		@param	buffer	buffer
		@param	size	the size of updated buffer
		@param	offset	the offset of updated buffer
		@param	data	updating data
		@return	Succeeded in updating?
	*/
	virtual bool UpdateIndexBuffer(IndexBuffer* buffer, int32_t size, int32_t offset, const void* data)
	{
		return false;
	}

	/**
		@brief	Create VertexLayout
		@param	elements	a pointer of array of vertex layout elements
		@param	elementCount	the number of elements
	*/
	virtual VertexLayout* CreateVertexLayout(const VertexLayoutElement* elements, int32_t elementCount)
	{
		return nullptr;
	}

	/**
		@brief	Create UniformBuffer
		@param	size	the size of buffer
		@param	initialData	the initial data of buffer. If it is null, not initialized.
		@return	UniformBuffer
	*/
	virtual UniformBuffer* CreateUniformBuffer(int32_t size, const void* initialData)
	{
		return nullptr;
	}

	virtual PipelineState* CreatePipelineState(PipelineStateParameter& param)
	{
		return nullptr;
	}

	virtual Texture* CreateTexture()
	{
		return nullptr;
	}

	virtual Texture* CreateRenderTexture()
	{
		return nullptr;
	}

	virtual Texture* CreateDepthTexture()
	{
		return nullptr;
	}

	/**
		@brief	Create Shader from key
		@param	key	a key which specifies a shader
		@return	Shader
	*/
	virtual Shader* CreateShaderFromKey(const char* key)
	{
		return nullptr;
	}

	/**
		@brief	Create ComputeBuffer
		@param	size	the size of buffer
		@param	initialData	the initial data of buffer. If it is null, not initialized.
		@return	ComputeBuffer
	*/
	virtual ComputeBuffer* CreateComputeBuffer(int32_t size, const void* initialData)
	{
		return nullptr;
	}

	virtual void Draw(DrawParameter& drawParam)
	{
	}
};

} // namespace Backend
} // namespace Effekseer

#endif
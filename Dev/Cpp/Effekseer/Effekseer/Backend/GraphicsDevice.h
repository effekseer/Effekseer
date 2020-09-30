
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

enum class UniformBufferLayoutElementType
{
	Vector4,
};

struct UniformLayoutElement
{
	std::string Name;
	UniformBufferLayoutElementType Type;

	//! Ignored in UniformBuffer
	int32_t Offset;
};

/**
	@brief	Layouts in an uniform buffer
	@note
	Only for OpenGL
*/
class UniformLayout
	: public ReferenceObject
{
private:
	CustomVector<UniformLayoutElement> elements_;

public:
	UniformLayout() = default;
	virtual ~UniformLayout() = default;

	const CustomVector<UniformLayoutElement>& GetElements() const
	{
		return elements_;
	}
};

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
private:
	UniformLayout* layout_ = nullptr;

public:
	UniformBuffer() = default;
	virtual ~UniformBuffer()
	{
		ES_SAFE_RELEASE(layout_);
	}

	UniformLayout* GetLayout() const
	{
		return layout_;	
	}

	void SetLayout(UniformLayout* layout)
	{
		ES_SAFE_ADDREF(layout);
		ES_SAFE_RELEASE(layout_);
		layout_ = layout;
	}
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
protected:
	TextureFormatType format_;
	std::array<int32_t, 2> size_;
	int32_t mipmapCount_;

public:
	Texture() = default;
	virtual ~Texture() = default;

	TextureFormatType GetFormat() const
	{
		return format_;
	}

	std::array<int32_t, 2> GetSize() const
	{
		return size_;
	}

	int32_t GetMipmapCount() const
	{
		return mipmapCount_;
	}
};

class Shader
	: public ReferenceObject
{
	UniformLayout* layout_ = nullptr;

public:
	Shader() = default;
	virtual ~Shader()
	{
		ES_SAFE_RELEASE(layout_);
	}
};

class ComputeBuffer
	: public ReferenceObject
{
public:
	ComputeBuffer() = default;
	virtual ~ComputeBuffer() = default;
};

class FrameBuffer
	: public ReferenceObject
{
public:
	FrameBuffer() = default;
	virtual ~FrameBuffer() = default;
};

class RenderPass
	: public ReferenceObject
{
public:
	RenderPass() = default;
	virtual ~RenderPass() = default;
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
	static const int TextureSlotCount = 8;

	VertexBuffer* VertexBufferPtr = nullptr;
	IndexBuffer* IndexBufferPtr = nullptr;
	PipelineState* PipelineStatePtr = nullptr;

	std::array<Texture*, TextureSlotCount> TexturePtrs;
	std::array<TextureWrapType, TextureSlotCount> TextureWrapTypes;
	std::array<TextureSamplingType, TextureSlotCount> TextureSamplingTypes;

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

	//! only for OpenGL
	std::string Name;

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

enum class CullingType
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

	CullingType Culling = CullingType::DoubleSide;

	bool IsBlendEnabled = true;

	BlendFuncType BlendSrcFunc = BlendFuncType::SrcAlpha;
	BlendFuncType BlendDstFunc = BlendFuncType::OneMinusSrcAlpha;
	BlendFuncType BlendSrcFuncAlpha = BlendFuncType::SrcAlpha;
	BlendFuncType BlendDstFuncAlpha = BlendFuncType::OneMinusSrcAlpha;

	BlendEquationType BlendEquationRGB = BlendEquationType::Add;
	BlendEquationType BlendEquationAlpha = BlendEquationType::Add;

	bool IsDepthTestEnabled = false;
	bool IsDepthWriteEnabled = false;
	DepthFuncType DepthFunc = DepthFuncType::Less;

	VertexLayout* VertexLayoutPtr = nullptr;
	FrameBuffer* FrameBufferPtr = nullptr;
};

struct TextureParameter
{
	TextureFormatType Format = TextureFormatType::R8G8B8A8_UNORM;
	bool GenerateMipmap = true;
	std::array<float, 2> Size;
	CustomVector<uint8_t> InitialData;
};

struct RenderTextureParameter
{
	TextureFormatType Format = TextureFormatType::R8G8B8A8_UNORM;
	std::array<float, 2> Size;
};

struct DepthTextureParameter
{
	TextureFormatType Format = TextureFormatType::R8G8B8A8_UNORM;
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
		@brief	Update content of a index buffer
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
		@brief	Update content of an uniform buffer
		@param	buffer	buffer
		@param	size	the size of updated buffer
		@param	offset	the offset of updated buffer
		@param	data	updating data
		@return	Succeeded in updating?
	*/
	virtual bool UpdateUniformBuffer(UniformBuffer* buffer, int32_t size, int32_t offset, const void* data)
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

	virtual PipelineState* CreatePipelineState(const PipelineStateParameter& param)
	{
		return nullptr;
	}

	virtual FrameBuffer* CreateFrameBuffer(const TextureFormatType* formats, int32_t formatCount, TextureFormatType* depthFormat)
	{
		return nullptr;
	}

	virtual RenderPass* CreateRenderPass(Texture** textures, int32_t textureCount, Texture* depthTexture)
	{
		return nullptr;
	}

	virtual Texture* CreateTexture(const TextureParameter& param)
	{
		return nullptr;
	}

	virtual Texture* CreateRenderTexture(const RenderTextureParameter& param)
	{
		return nullptr;
	}

	virtual Texture* CreateDepthTexture(const DepthTextureParameter& param)
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
	// virtual ComputeBuffer* CreateComputeBuffer(int32_t size, const void* initialData)
	// {
	// 	return nullptr;
	// }

	virtual void Draw(const DrawParameter& drawParam)
	{
	}
};

} // namespace Backend
} // namespace Effekseer

#endif
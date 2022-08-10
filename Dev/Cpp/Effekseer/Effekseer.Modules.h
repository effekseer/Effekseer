﻿#pragma once

#include "Effekseer.h"
#include "Effekseer.SIMD.h"

// A header to access internal data of effekseer

#ifndef __EFFEKSEER_PARAMETERS_H__
#define __EFFEKSEER_PARAMETERS_H__


namespace Effekseer
{

void LoadGradient(Gradient& gradient, uint8_t*& pos, int32_t version);

/**
	@brief
	\~English	How to treat an uv of texture type for ribbon and track
	\~Japanese	リボンと軌跡向けのテクスチャのUVの扱い方
*/
enum class TextureUVType : int32_t
{
	Strech,
	Tile,
};

/**
	@brief	\~english	uv texture parameters which is passed into a renderer
			\~japanese	レンダラーに渡されるUVTextureに関するパラメーター
*/
struct NodeRendererTextureUVTypeParameter
{
	TextureUVType Type = TextureUVType::Strech;
	float TileLength = 0.0f;
	int32_t TileEdgeHead = 0;
	int32_t TileEdgeTail = 0;
	float TileLoopAreaBegin = 0.0f;
	float TileLoopAreaEnd = 0.0f;

	void Load(uint8_t*& pos, int32_t version);
};

struct FalloffParameter
{
	enum BlendType
	{
		Add = 0,
		Sub = 1,
		Mul = 2,
	} ColorBlendType;
	Color BeginColor;
	Color EndColor;
	float Pow;

	FalloffParameter()
	{
		ColorBlendType = BlendType::Add;
		BeginColor = Color(255, 255, 255, 255);
		EndColor = Color(255, 255, 255, 255);
		Pow = 1.0f;
	}
};

} // namespace Effekseer

#endif // __EFFEKSEER_PARAMETERS_H__

#ifndef __EFFEKSEER_SPRITE_RENDERER_H__
#define __EFFEKSEER_SPRITE_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class SpriteRenderer : public ReferenceObject
{
public:
	struct NodeParameter
	{
		float LocalTime;
		Effect* EffectPointer;
		bool ZTest;
		bool ZWrite;
		BillboardType Billboard;
		bool IsRightHand;
		float Maginification = 1.0f;

		ZSortType ZSort;

		NodeRendererDepthParameter* DepthParameterPtr = nullptr;
		NodeRendererBasicParameter* BasicParameterPtr = nullptr;

		bool EnableViewOffset = false;

		RefPtr<RenderingUserData> UserData;
	};

	struct InstanceParameter
	{
		SIMD::Mat43f SRTMatrix43;
		Color AllColor;

		// Lower left, Lower right, Upper left, Upper right
		Color Colors[4];

		SIMD::Vec2f Positions[4];

		RectF UV;

		RectF AlphaUV;

		RectF UVDistortionUV;

		RectF BlendUV;

		RectF BlendAlphaUV;

		RectF BlendUVDistortionUV;

		float FlipbookIndexAndNextRate;

		float AlphaThreshold;

		float ViewOffsetDistance;

		std::array<float, 4> CustomData1;
		std::array<float, 4> CustomData2;
	};

public:
	SpriteRenderer()
	{
	}

	virtual ~SpriteRenderer()
	{
	}

	virtual void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void Rendering(const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData)
	{
	}

	virtual void EndRendering(const NodeParameter& parameter, void* userData)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_SPRITE_RENDERER_H__

#ifndef __EFFEKSEER_RIBBON_RENDERER_H__
#define __EFFEKSEER_RIBBON_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

struct NodeRendererTextureUVTypeParameter;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class RibbonRenderer : public ReferenceObject
{
public:
	struct NodeParameter
	{
		float LocalTime;
		Effect* EffectPointer;
		bool ZTest;
		bool ZWrite;
		bool ViewpointDependent;

		bool IsRightHand;
		float Maginification = 1.0f;

		int32_t SplineDivision;
		NodeRendererDepthParameter* DepthParameterPtr = nullptr;
		NodeRendererBasicParameter* BasicParameterPtr = nullptr;
		NodeRendererTextureUVTypeParameter* TextureUVTypeParameterPtr = nullptr;

		bool EnableViewOffset = false;

		RefPtr<RenderingUserData> UserData;
	};

	struct InstanceParameter
	{
		int32_t InstanceCount;
		int32_t InstanceIndex;
		SIMD::Mat43f SRTMatrix43;
		Color AllColor;

		// Lower left, Lower right, Upper left, Upper right
		Color Colors[4];

		float Positions[4];

		RectF UV;

		RectF AlphaUV;

		RectF UVDistortionUV;

		RectF BlendUV;

		RectF BlendAlphaUV;

		RectF BlendUVDistortionUV;

		float FlipbookIndexAndNextRate;

		float AlphaThreshold;

		float ViewOffsetDistance;

		std::array<float, 4> CustomData1;
		std::array<float, 4> CustomData2;
	};

public:
	RibbonRenderer()
	{
	}

	virtual ~RibbonRenderer()
	{
	}

	virtual void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void Rendering(const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData)
	{
	}

	virtual void EndRendering(const NodeParameter& parameter, void* userData)
	{
	}

	virtual void BeginRenderingGroup(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void EndRenderingGroup(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_RIBBON_RENDERER_H__

#ifndef __EFFEKSEER_RING_RENDERER_H__
#define __EFFEKSEER_RING_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class RingRenderer : public ReferenceObject
{
public:
	struct NodeParameter
	{
		float LocalTime;
		Effect* EffectPointer;
		bool ZTest;
		bool ZWrite;
		BillboardType Billboard;
		int32_t VertexCount;
		bool IsRightHand;
		float Maginification = 1.0f;

		float StartingFade = 0.0f;
		float EndingFade = 0.0f;

		NodeRendererDepthParameter* DepthParameterPtr = nullptr;
		NodeRendererBasicParameter* BasicParameterPtr = nullptr;

		NodeRendererBasicParameter BasicParameter;

		bool EnableViewOffset = false;

		RefPtr<RenderingUserData> UserData;
	};

	struct InstanceParameter
	{
		SIMD::Mat43f SRTMatrix43;
		SIMD::Vec2f OuterLocation;
		SIMD::Vec2f InnerLocation;
		float ViewingAngleStart;
		float ViewingAngleEnd;
		float CenterRatio;
		Color OuterColor;
		Color CenterColor;
		Color InnerColor;

		RectF UV;

		RectF AlphaUV;

		RectF UVDistortionUV;

		RectF BlendUV;

		RectF BlendAlphaUV;

		RectF BlendUVDistortionUV;

		float FlipbookIndexAndNextRate;

		float AlphaThreshold;

		float ViewOffsetDistance;

		std::array<float, 4> CustomData1;
		std::array<float, 4> CustomData2;
	};

public:
	RingRenderer()
	{
	}

	virtual ~RingRenderer()
	{
	}

	virtual void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void Rendering(const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData)
	{
	}

	virtual void EndRendering(const NodeParameter& parameter, void* userData)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_RING_RENDERER_H__

#ifndef __EFFEKSEER_MODEL_RENDERER_H__
#define __EFFEKSEER_MODEL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class ModelRenderer : public ReferenceObject
{
public:
	struct NodeParameter
	{
		float LocalTime;
		Effect* EffectPointer;
		bool ZTest;
		bool ZWrite;
		BillboardType Billboard;

		// bool				Lighting;
		CullingType Culling;
		int32_t ModelIndex;
		float Magnification;
		bool IsRightHand;
		float Maginification = 1.0f;

		NodeRendererDepthParameter* DepthParameterPtr = nullptr;
		NodeRendererBasicParameter* BasicParameterPtr = nullptr;

		bool EnableFalloff;
		FalloffParameter FalloffParam;

		bool EnableViewOffset = false;

		bool IsProceduralMode = false;

		RefPtr<RenderingUserData> UserData;
	};

	struct InstanceParameter
	{
		SIMD::Mat43f SRTMatrix43;
		RectF UV;

		RectF AlphaUV;

		RectF UVDistortionUV;

		RectF BlendUV;

		RectF BlendAlphaUV;

		RectF BlendUVDistortionUV;

		float FlipbookIndexAndNextRate;

		float AlphaThreshold;

		float ViewOffsetDistance;

		Color AllColor;
		int32_t Time;
		std::array<float, 4> CustomData1;
		std::array<float, 4> CustomData2;
	};

public:
	ModelRenderer()
	{
	}

	virtual ~ModelRenderer()
	{
	}

	virtual void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void Rendering(const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData)
	{
	}

	virtual void EndRendering(const NodeParameter& parameter, void* userData)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_MODEL_RENDERER_H__

#ifndef __EFFEKSEER_TRACK_RENDERER_H__
#define __EFFEKSEER_TRACK_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

struct NodeRendererTextureUVTypeParameter;

class TrackRenderer : public ReferenceObject
{
public:
	struct NodeParameter
	{
		float LocalTime;
		Effect* EffectPointer;
		bool ZTest;
		bool ZWrite;

		int32_t SplineDivision;

		bool IsRightHand;
		float Maginification = 1.0f;

		NodeRendererDepthParameter* DepthParameterPtr = nullptr;
		NodeRendererBasicParameter* BasicParameterPtr = nullptr;
		NodeRendererTextureUVTypeParameter* TextureUVTypeParameterPtr = nullptr;

		RendererMaterialType MaterialType = RendererMaterialType::Default;
		MaterialRenderData* MaterialRenderDataPtr = nullptr;

		bool EnableViewOffset = false;

		TrailSmoothingType SmoothingType = TrailSmoothingType::Off;

		RefPtr<RenderingUserData> UserData;
	};

	struct InstanceGroupParameter
	{
	};

	struct InstanceParameter
	{
		int32_t InstanceCount;
		int32_t InstanceIndex;
		SIMD::Mat43f SRTMatrix43;

		Color ColorLeft;
		Color ColorCenter;
		Color ColorRight;

		Color ColorLeftMiddle;
		Color ColorCenterMiddle;
		Color ColorRightMiddle;

		float SizeFor;
		float SizeMiddle;
		float SizeBack;

		RectF UV;

		RectF AlphaUV;

		RectF UVDistortionUV;

		RectF BlendUV;

		RectF BlendAlphaUV;

		RectF BlendUVDistortionUV;

		float FlipbookIndexAndNextRate;

		float AlphaThreshold;

		float ViewOffsetDistance;

		std::array<float, 4> CustomData1;
		std::array<float, 4> CustomData2;
	};

public:
	TrackRenderer()
	{
	}

	virtual ~TrackRenderer()
	{
	}

	virtual void BeginRendering(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void Rendering(const NodeParameter& parameter, const InstanceParameter& instanceParameter, void* userData)
	{
	}

	virtual void EndRendering(const NodeParameter& parameter, void* userData)
	{
	}

	virtual void BeginRenderingGroup(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}

	virtual void EndRenderingGroup(const NodeParameter& parameter, int32_t count, void* userData)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_TRACK_RENDERER_H__

#ifndef __EFFEKSEER_EFFECTLOADER_H__
#define __EFFEKSEER_EFFECTLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	エフェクトファイル読み込み破棄関数指定クラス
*/
class EffectLoader : public ReferenceObject
{
public:
	/**
		@brief	コンストラクタ
	*/
	EffectLoader()
	{
	}

	/**
		@brief	デストラクタ
	*/
	virtual ~EffectLoader()
	{
	}

	/**
		@brief	エフェクトファイルを読み込む。
		@param	path	[in]	読み込み元パス
		@param	data	[out]	データ配列の先頭のポインタを出力する先
		@param	size	[out]	データ配列の長さを出力する先
		@return	成否
		@note
		エフェクトファイルを読み込む。
		::Effekseer::Effect::Create実行時に使用される。
	*/
	virtual bool Load(const char16_t* path, void*& data, int32_t& size) = 0;

	/**
		@brief	エフェクトファイルを破棄する。
		@param	data	[in]	データ配列の先頭のポインタ
		@param	size	[int]	データ配列の長さ
		@note
		エフェクトファイルを破棄する。
		::Effekseer::Effect::Create実行終了時に使用される。
	*/
	virtual void Unload(void* data, int32_t size) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_EFFECTLOADER_H__

#ifndef __EFFEKSEER_TEXTURELOADER_H__
#define __EFFEKSEER_TEXTURELOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	テクスチャ読み込み破棄関数指定クラス
*/
class TextureLoader : public ReferenceObject
{
public:
	/**
		@brief	コンストラクタ
	*/
	TextureLoader()
	{
	}

	/**
		@brief	デストラクタ
	*/
	virtual ~TextureLoader()
	{
	}

	/**
		@brief	テクスチャを読み込む。
		@param	path	[in]	読み込み元パス
		@param	textureType	[in]	テクスチャの種類
		@return	テクスチャのポインタ
		@note
		テクスチャを読み込む。
		::Effekseer::Effect::Create実行時に使用される。
	*/
	virtual TextureRef Load(const char16_t* path, TextureType textureType)
	{
		return nullptr;
	}

	/**
		@brief
		\~English	a function called when texture is loaded
		\~Japanese	テクスチャが読み込まれるときに呼ばれる関数
		@param	data
		\~English	data pointer
		\~Japanese	データのポインタ
		@param	size
		\~English	the size of data
		\~Japanese	データの大きさ
		@param	textureType
		\~English	a kind of texture
		\~Japanese	テクスチャの種類
		@param	isMipMapEnabled
		\~English	whether is a mipmap enabled
		\~Japanese	MipMapが有効かどうか
		@return
		\~English	a pointer of loaded texture
		\~Japanese	読み込まれたテクスチャのポインタ
	*/
	virtual TextureRef Load(const void* data, int32_t size, TextureType textureType, bool isMipMapEnabled)
	{
		return nullptr;
	}

	/**
		@brief	テクスチャを破棄する。
		@param	data	[in]	テクスチャ
		@note
		テクスチャを破棄する。
		::Effekseer::Effectのインスタンスが破棄された時に使用される。
	*/
	virtual void Unload(TextureRef data)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_TEXTURELOADER_H__

#ifndef __EFFEKSEER_MODEL_H__
#define __EFFEKSEER_MODEL_H__


namespace Effekseer
{

namespace Backend
{
class GraphicsDevice;
class VertexBuffer;
class IndexBuffer;
} // namespace Backend

/**
	@brief
	\~English	Model class
	\~Japanese	モデルクラス
*/
class Model : public Resource
{
public:
	static const int32_t Version = 1;
	static const int32_t LastetVersion = 5;

	struct Vertex
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
		Vector2D UV;
		Color VColor;
	};

	struct Face
	{
		std::array<int32_t, 3> Indexes;
	};

	struct Emitter
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
	};

protected:
	struct InternalModel
	{
		CustomVector<Vertex> vertexes;
		CustomVector<Face> faces;
		RefPtr<Backend::VertexBuffer> vertexBuffer;
		RefPtr<Backend::IndexBuffer> indexBuffer;
		RefPtr<Backend::IndexBuffer> wireIndexBuffer;
	};

	int32_t version_ = 0;
	CustomVector<InternalModel> models_;
	bool isBufferStoredOnGPU_ = false;
	bool isWireIndexBufferGenerated_ = false;

public:
	Model(const CustomVector<Vertex>& vertecies, const CustomVector<Face>& faces);

	Model(const void* data, int32_t size);

	virtual ~Model();

	const RefPtr<Backend::VertexBuffer>& GetVertexBuffer(int32_t index) const;

	const RefPtr<Backend::IndexBuffer>& GetIndexBuffer(int32_t index) const;

	const RefPtr<Backend::IndexBuffer>& GetWireIndexBuffer(int32_t index) const;

	const Vertex* GetVertexes(int32_t index = 0) const;

	int32_t GetVertexCount(int32_t index = 0) const;

	const Face* GetFaces(int32_t index = 0) const;

	int32_t GetFaceCount(int32_t index = 0) const;

	int32_t GetFrameCount() const;

	Emitter GetEmitter(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification);

	Emitter GetEmitterFromVertex(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification);

	Emitter GetEmitterFromVertex(int32_t index, int32_t time, CoordinateSystem coordinate, float magnification);

	Emitter GetEmitterFromFace(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification);

	Emitter GetEmitterFromFace(int32_t index, int32_t time, CoordinateSystem coordinate, float magnification);

	bool StoreBufferToGPU(Backend::GraphicsDevice* graphicsDevice, bool flipVertexColor = false);

	bool GetIsBufferStoredOnGPU() const;

	bool GenerateWireIndexBuffer(Backend::GraphicsDevice* graphicsDevice);

	bool GetIsWireIndexBufferGenerated() const;
};

} // namespace Effekseer

#endif // __EFFEKSEER_MODEL_H__

#ifndef __EFFEKSEER_MODELLOADER_H__
#define __EFFEKSEER_MODELLOADER_H__


namespace Effekseer
{

/**
	\~English	Model loader
	\~Japanese	モデル読み込み破棄関数指定クラス
*/
class ModelLoader : public ReferenceObject
{
public:
	ModelLoader() = default;

	virtual ~ModelLoader() = default;

	/*
	@brief
	\~English load a model
	\~Japanese モデルを読み込む。
	@param path
	\~English a file path
	\~Japanese 読み込み元パス
	@ return
	\~English a pointer of loaded a model
	\~Japanese 読み込まれたモデルのポインタ
	*/
	virtual ModelRef Load(const char16_t* path);

	/**
		@brief
		\~English	a function called when model is loaded
		\~Japanese	モデルが読み込まれるときに呼ばれる関数
		@param	data
		\~English	data pointer
		\~Japanese	データのポインタ
		@param	size
		\~English	the size of data
		\~Japanese	データの大きさ
		@return
		\~English	a pointer of loaded model
		\~Japanese	読み込まれたモデルのポインタ
	*/
	virtual ModelRef Load(const void* data, int32_t size);

	/**
		@brief
		\~English	dispose a model
		\~Japanese	モデルを破棄する。
		@param	data
		\~English	a pointer of loaded a model
		\~Japanese	読み込まれたモデルのポインタ
	*/
	virtual void Unload(ModelRef data);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_MODELLOADER_H__

#ifndef __EFFEKSEER_MATERIALLOADER_H__
#define __EFFEKSEER_MATERIALLOADER_H__


namespace Effekseer
{

/**
	@brief
	\~English	Material loader
	\~Japanese	マテリアル読み込み破棄関数指定クラス
*/
class MaterialLoader : public ReferenceObject
{
public:
	/**
	@brief
	\~English	Constructor
	\~Japanese	コンストラクタ
	*/
	MaterialLoader() = default;

	/**
	@brief
	\~English	Destructor
	\~Japanese	デストラクタ
	*/
	virtual ~MaterialLoader() = default;

	/**
		@brief
		\~English	load a material
		\~Japanese	マテリアルを読み込む。
		@param	path
		\~English	a file path
		\~Japanese	読み込み元パス
		@return
		\~English	a pointer of loaded a material
		\~Japanese	読み込まれたマテリアルのポインタ
	*/
	virtual MaterialRef Load(const char16_t* path)
	{
		return nullptr;
	}

	/**
		@brief
		\~English	a function called when a material is loaded
		\~Japanese	マテリアルが読み込まれるときに呼ばれる関数
		@param	data
		\~English	data pointer
		\~Japanese	データのポインタ
		@param	size
		\~English	the size of data
		\~Japanese	データの大きさ
		@param	fileType
		\~English	file type
		\~Japanese	ファイルの種類
		@return
		\~English	a pointer of loaded a material
		\~Japanese	読み込まれたマテリアルのポインタ
	*/
	virtual MaterialRef Load(const void* data, int32_t size, MaterialFileType fileType)
	{
		return nullptr;
	}

	/**
		@brief
		\~English	dispose a material
		\~Japanese	マテリアルを破棄する。
		@param	data
		\~English	a pointer of loaded a material
		\~Japanese	読み込まれたマテリアルのポインタ
	*/
	virtual void Unload(MaterialRef data)
	{
	}
};

} // namespace Effekseer

#endif // __EFFEKSEER_TEXTURELOADER_H__

#ifndef __EFFEKSEER_MODEL_H__
#define __EFFEKSEER_MODEL_H__


namespace Effekseer
{

namespace Backend
{
class GraphicsDevice;
class VertexBuffer;
class IndexBuffer;
} // namespace Backend

/**
	@brief
	\~English	Model class
	\~Japanese	モデルクラス
*/
class Model : public Resource
{
public:
	static const int32_t Version = 1;
	static const int32_t LastetVersion = 5;

	struct Vertex
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
		Vector2D UV;
		Color VColor;
	};

	struct Face
	{
		std::array<int32_t, 3> Indexes;
	};

	struct Emitter
	{
		Vector3D Position;
		Vector3D Normal;
		Vector3D Binormal;
		Vector3D Tangent;
	};

protected:
	struct InternalModel
	{
		CustomVector<Vertex> vertexes;
		CustomVector<Face> faces;
		RefPtr<Backend::VertexBuffer> vertexBuffer;
		RefPtr<Backend::IndexBuffer> indexBuffer;
		RefPtr<Backend::IndexBuffer> wireIndexBuffer;
	};

	int32_t version_ = 0;
	CustomVector<InternalModel> models_;
	bool isBufferStoredOnGPU_ = false;
	bool isWireIndexBufferGenerated_ = false;

public:
	Model(const CustomVector<Vertex>& vertecies, const CustomVector<Face>& faces);

	Model(const void* data, int32_t size);

	virtual ~Model();

	const RefPtr<Backend::VertexBuffer>& GetVertexBuffer(int32_t index) const;

	const RefPtr<Backend::IndexBuffer>& GetIndexBuffer(int32_t index) const;

	const RefPtr<Backend::IndexBuffer>& GetWireIndexBuffer(int32_t index) const;

	const Vertex* GetVertexes(int32_t index = 0) const;

	int32_t GetVertexCount(int32_t index = 0) const;

	const Face* GetFaces(int32_t index = 0) const;

	int32_t GetFaceCount(int32_t index = 0) const;

	int32_t GetFrameCount() const;

	Emitter GetEmitter(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification);

	Emitter GetEmitterFromVertex(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification);

	Emitter GetEmitterFromVertex(int32_t index, int32_t time, CoordinateSystem coordinate, float magnification);

	Emitter GetEmitterFromFace(IRandObject* g, int32_t time, CoordinateSystem coordinate, float magnification);

	Emitter GetEmitterFromFace(int32_t index, int32_t time, CoordinateSystem coordinate, float magnification);

	bool StoreBufferToGPU(Backend::GraphicsDevice* graphicsDevice, bool flipVertexColor = false);

	bool GetIsBufferStoredOnGPU() const;

	bool GenerateWireIndexBuffer(Backend::GraphicsDevice* graphicsDevice);

	bool GetIsWireIndexBufferGenerated() const;
};

} // namespace Effekseer

#endif // __EFFEKSEER_MODEL_H__

#ifndef __EFFEKSEER_CURVE_H__
#define __EFFEKSEER_CURVE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

#include <cmath>
#include <limits>
#include <vector>

namespace Effekseer
{

class dVector4
{
public:
	double X, Y, Z, W;

public:
	dVector4(double x = 0, double y = 0, double z = 0, double w = 0)
		: X(x)
		, Y(y)
		, Z(z)
		, W(w)
	{
	}
};

/**
@brief
\~English	Curve class
\~Japanese	カーブクラス
*/
class Curve : public Resource
{
	friend class CurveLoader;

public:
	static const int32_t Version = 1;

private:
	int mControllPointCount;
	std::vector<dVector4> mControllPoint;

	int mKnotCount;
	std::vector<double> mKnotValue;

	int mOrder;
	int mStep;
	int mType;
	int mDimension;

	float mLength;

private:
	/**
	 * CalcBSplineBasisFunc : B-スプライン基底関数の計算
	 * 
	 * const vector<double>& knot : ノット列
	 * unsigned int j : ノット列の開始番号
	 * unsigned int p : 次数
	 * double t : 計算対象の独立変数
	 * 
	 * ノット列は昇順である必要があるが、そのチェックは行わない
	 * 
	 * 戻り値 : 計算結果
	 */
	double CalcBSplineBasisFunc(const std::vector<double>& knot, unsigned int j, unsigned int p, double t)
	{
		if (knot.size() == 0)
			return std::numeric_limits<double>::quiet_NaN();

		// ノット列のデータ長が充分でない場合は nan を返す
		unsigned int m = static_cast<unsigned int>(knot.size()) - 1;
		if (m < j + p + 1)
			return std::numeric_limits<double>::quiet_NaN();

		// 正値をとる範囲外ならゼロを返す
		if ((t < knot[j]) || (t > knot[j + p + 1]))
			return (0);
		// p = 0 かつ knot[j] <= t <= knot[j + p + 1] なら 1 を返す
		if (p == 0)
			return (1);
		// p = 1 の場合、三角の頂点の値は特別扱い
		if (p == 1 && t == knot[j + 1])
			return (1);

		// 漸化式の計算
		double d1 = (knot[j + p] == knot[j]) ? 0 : (t - knot[j]) * CalcBSplineBasisFunc(knot, j, p - 1, t) / (knot[j + p] - knot[j]);
		double d2 = (knot[j + p + 1] == knot[j + 1]) ? 0 : (knot[j + p + 1] - t) * CalcBSplineBasisFunc(knot, j + 1, p - 1, t) / (knot[j + p + 1] - knot[j + 1]);

		return (d1 + d2);
	}

public:
	Curve()
	{
	}

	Curve(const void* data, int32_t size)
	{
		uint8_t* pData = new uint8_t[size];
		memcpy(pData, data, size);

		uint8_t* p = (uint8_t*)pData;

		// load converter version
		int converter_version = 0;
		memcpy(&converter_version, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load controll point count
		memcpy(&mControllPointCount, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load controll points
		for (int i = 0; i < mControllPointCount; i++)
		{
			dVector4 value;
			memcpy(&value, p, sizeof(dVector4));
			p += sizeof(dVector4);
			mControllPoint.push_back(value);
		}

		// load knot count
		memcpy(&mKnotCount, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load knot values
		for (int i = 0; i < mKnotCount; i++)
		{
			double value;
			memcpy(&value, p, sizeof(double));
			p += sizeof(double);
			mKnotValue.push_back(value);
		}

		// load order
		memcpy(&mOrder, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load step
		memcpy(&mStep, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load type
		memcpy(&mType, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// load dimension
		memcpy(&mDimension, p, sizeof(int32_t));
		p += sizeof(int32_t);

		// calc curve length
		mLength = 0;

		for (int i = 1; i < mControllPointCount; i++)
		{
			dVector4 p0 = mControllPoint[i - 1];
			dVector4 p1 = mControllPoint[i];

			float len = Vector3D::Length(Vector3D((float)p1.X, (float)p1.Y, (float)p1.Z) - Vector3D((float)p0.X, (float)p0.Y, (float)p0.Z));
			mLength += len;
		}

		ES_SAFE_DELETE_ARRAY(pData);
	}

	~Curve()
	{
	}

	Vector3D CalcuratePoint(float t, float magnification)
	{
		if (t == 0.0f && mControllPoint.size() > 0)
		{
			return {
				static_cast<float>(mControllPoint[0].X * magnification),
				static_cast<float>(mControllPoint[0].Y * magnification),
				static_cast<float>(mControllPoint[0].Z * magnification)};
		}

		int p = mOrder; // 次数

		std::vector<double> bs(mControllPointCount); // B-Spline 基底関数の計算結果(重み値を積算)

		// ノット列の要素を +1 する
		auto knot = mKnotValue;
		knot.push_back(mKnotValue[mKnotValue.size() - 1] + 1);

		float t_rate = float(knot.back() - 1);

		double wSum = 0; // bs の合計
		for (int j = 0; j < mControllPointCount; ++j)
		{
			bs[j] = mControllPoint[j].W * CalcBSplineBasisFunc(knot, j, p, t * (t_rate));

			if (!std::isnan(bs[j]))
			{
				wSum += bs[j];
			}
		}

		Vector3D ans(0, 0, 0); // 計算結果
		for (int j = 0; j < mControllPointCount; ++j)
		{
			Vector3D d;
			d.X = (float)mControllPoint[j].X * magnification * (float)bs[j] / (float)wSum;
			d.Y = (float)mControllPoint[j].Y * magnification * (float)bs[j] / (float)wSum;
			d.Z = (float)mControllPoint[j].Z * magnification * (float)bs[j] / (float)wSum;
			if (!std::isnan(d.X) && !std::isnan(d.Y) && !std::isnan(d.Z))
			{
				ans += d;
			}
		}

		return ans;
	}

	//
	//  Getter
	//
	int GetControllPointCount()
	{
		return mControllPointCount;
	}
	dVector4 GetControllPoint(int index)
	{
		return mControllPoint[index];
	}

	int GetKnotCount()
	{
		return mKnotCount;
	}
	double GetKnotValue(int index)
	{
		return mKnotValue[index];
	}

	int GetOrder()
	{
		return mOrder;
	}
	int GetStep()
	{
		return mStep;
	}
	int GetType()
	{
		return mType;
	}
	int GetDimension()
	{
		return mDimension;
	}

	float GetLength()
	{
		return mLength;
	}

}; // end class

} // end namespace Effekseer

#endif // __EFFEKSEER_CURVE_H__

#ifndef __EFFEKSEER_CURVELOADER_H__
#define __EFFEKSEER_CURVELOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

#include <memory>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

/**
	\~English	Curve loader
	\~Japanese	カーブ読み込み破棄関数指定クラス
*/
class CurveLoader : public ReferenceObject
{
private:
	::Effekseer::FileInterfaceRef fileInterface_ = nullptr;

public:
	CurveLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr);

	virtual ~CurveLoader() = default;

	/*
	@brief
	\~English load a curve
	\~Japanese カーブを読み込む。
	@param path
	\~English a file path
	\~Japanese 読み込み元パス
	@ return
	\~English a pointer of loaded a curve
	\~Japanese 読み込まれたカーブのポインタ
	*/
	virtual CurveRef Load(const char16_t* path);

	/*
	@brief
	\~English load a curve
	\~Japanese カーブを読み込む。
	@param	data
	\~English	data pointer
	\~Japanese	データのポインタ
	@param	size
	\~English	the size of data
	\~Japanese	データの大きさ
	@ return
	\~English a pointer of loaded a curve
	\~Japanese 読み込まれたカーブのポインタ
	*/
	virtual CurveRef Load(const void* data, int32_t size);

	/**
		@brief
		\~English	dispose a curve
		\~Japanese	カーブを破棄する。
		@param	data
		\~English	a pointer of loaded a curve
		\~Japanese	読み込まれたカーブのポインタ
	*/
	virtual void Unload(CurveRef data);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
  //----------------------------------------------------------------------------------
  //
  //----------------------------------------------------------------------------------
#endif // __EFFEKSEER_MODELLOADER_H__

#ifndef	__EFFEKSEER_SOUND_PLAYER_H__
#define	__EFFEKSEER_SOUND_PLAYER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

typedef void* SoundHandle;
typedef void* SoundTag;

class SoundPlayer : public ReferenceObject
{
public:
	struct InstanceParameter
	{
		SoundDataRef	Data;
		float		Volume;
		float		Pan;
		float		Pitch;
		bool		Mode3D;
		Vector3D	Position;
		float		Distance;
		void*		UserData;
	};

public:
	SoundPlayer() {}

	virtual ~SoundPlayer() {}

	virtual SoundHandle Play( SoundTag tag, const InstanceParameter& parameter ) = 0;
	
	virtual void Stop( SoundHandle handle, SoundTag tag ) = 0;

	virtual void Pause( SoundHandle handle, SoundTag tag, bool pause ) = 0;

	virtual bool CheckPlaying( SoundHandle handle, SoundTag tag ) = 0;

	virtual void StopTag( SoundTag tag ) = 0;

	virtual void PauseTag( SoundTag tag, bool pause ) = 0;

	virtual bool CheckPlayingTag( SoundTag tag ) = 0;

	virtual void StopAll() = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SOUND_PLAYER_H__

#ifndef __EFFEKSEER_SOUNDLOADER_H__
#define __EFFEKSEER_SOUNDLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	サウンドデータ
*/
class SoundData : public Resource
{
public:
	explicit SoundData() = default;
	virtual ~SoundData() = default;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	サウンド読み込み破棄関数指定クラス
*/
class SoundLoader : public ReferenceObject
{
public:
	/**
		@brief	コンストラクタ
	*/
	SoundLoader()
	{
	}

	/**
		@brief	デストラクタ
	*/
	virtual ~SoundLoader()
	{
	}

	/**
		@brief	サウンドを読み込む。
		@param	path	[in]	読み込み元パス
		@return	サウンドのポインタ
		@note
		サウンドを読み込む。
		::Effekseer::Effect::Create実行時に使用される。
	*/
	virtual SoundDataRef Load(const char16_t* path)
	{
		return nullptr;
	}

	/**
		@brief
		\~English	a function called when sound is loaded
		\~Japanese	サウンドが読み込まれるときに呼ばれる関数
		@param	data
		\~English	data pointer
		\~Japanese	データのポインタ
		@param	size
		\~English	the size of data
		\~Japanese	データの大きさ
		@return
		\~English	a pointer of loaded texture
		\~Japanese	読み込まれたサウンドのポインタ
	*/
	virtual SoundDataRef Load(const void* data, int32_t size)
	{
		return nullptr;
	}

	/**
		@brief	サウンドを破棄する。
		@param	data	[in]	サウンド
		@note
		サウンドを破棄する。
		::Effekseer::Effectのインスタンスが破棄された時に使用される。
	*/
	virtual void Unload(SoundDataRef data)
	{
		data.Reset();
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_SOUNDLOADER_H__

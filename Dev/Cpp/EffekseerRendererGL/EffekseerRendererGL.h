
#ifndef __EFFEKSEERRENDERER_GL_BASE_PRE_H__
#define __EFFEKSEERRENDERER_GL_BASE_PRE_H__

#include <Effekseer.h>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

#if defined(__EFFEKSEER_RENDERER_GLES2__)

#if defined(__APPLE__)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#elif defined(__EFFEKSEER_RENDERER_GLES3__)

#if defined(__APPLE__)
#include <OpenGLES/ES3/gl.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3.h>
#endif

#elif defined(__EFFEKSEER_RENDERER_GL2__)

#if _WIN32
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#else

#if _WIN32
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#endif

#if _WIN32
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "opengl32.lib")
#endif

namespace EffekseerRendererGL
{

class Renderer;

enum class OpenGLDeviceType
{
	OpenGL2,
	OpenGL3,
	OpenGLES2,
	OpenGLES3,
	Emscripten,
};

} // namespace EffekseerRendererGL

#endif // __EFFEKSEERRENDERER_GL_BASE_PRE_H__

#ifndef __EFFEKSEERRENDERER_GL_DEVICEOBJECT_COLLECTION_H__
#define __EFFEKSEERRENDERER_GL_DEVICEOBJECT_COLLECTION_H__

#include <Effekseer.h>
#include <set>

namespace EffekseerRendererGL
{

class DeviceObject;

class DeviceObjectCollection : public ::Effekseer::ReferenceObject
{
	friend class DeviceObject;

private:
	std::set<DeviceObject*> deviceObjects_;

	/**
		@brief	register an object
	*/
	void Register(DeviceObject* device);

	/**
		@brief	unregister an object
	*/
	void Unregister(DeviceObject* device);

public:
	DeviceObjectCollection() = default;

	~DeviceObjectCollection() = default;

	/**
		@brief
		\~english Call when device lost causes
		\~japanese デバイスロストが発生した時に実行する。
	*/
	void OnLostDevice();

	/**
		@brief
		\~english Call when device reset causes
		\~japanese デバイスがリセットされた時に実行する。
	*/
	void OnResetDevice();
};

} // namespace EffekseerRendererGL

#endif // __EFFEKSEERRENDERER_GL_DEVICEOBJECT_H__
#ifndef __EFFEKSEERRENDERER_RENDERER_H__
#define __EFFEKSEERRENDERER_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

/**
	@brief	背景を歪ませるエフェクトを描画する前に実行されるコールバック
*/
class DistortingCallback
{
public:
	DistortingCallback()
	{
	}
	virtual ~DistortingCallback()
	{
	}

	virtual bool OnDistorting()
	{
		return false;
	}
};

/**
	@brief	
	\~english A status of UV when particles are rendered.
	\~japanese パーティクルを描画する時のUVの状態
*/
enum class UVStyle
{
	Normal,
	VerticalFlipped,
};

/**
	@brief
	\~english A type of texture which is rendered when textures are not assigned.
	\~japanese テクスチャが設定されていないときに描画されるテクスチャの種類
*/
enum class ProxyTextureType
{
	White,
	Normal,
};

/**
	@brief
	\~english A class which contains a graphics device
	\~japanese グラフィックデバイスを格納しているクラス
*/
class GraphicsDevice : public ::Effekseer::IReference
{
public:
	GraphicsDevice() = default;
	virtual ~GraphicsDevice() = default;
};

class CommandList : public ::Effekseer::IReference
{
public:
	CommandList() = default;
	virtual ~CommandList() = default;
};

class SingleFrameMemoryPool : public ::Effekseer::IReference
{
public:
	SingleFrameMemoryPool() = default;
	virtual ~SingleFrameMemoryPool() = default;

	/**
		@brief
		\~English	notify that new frame is started.
		\~Japanese	新規フレームが始ったことを通知する。
	*/
	virtual void NewFrame()
	{
	}
};

class Renderer
	: public ::Effekseer::IReference
{
protected:
	Renderer();
	virtual ~Renderer();

	class Impl;
	Impl* impl = nullptr;

public:
	/**
		@brief	only for Effekseer backend developer. Effekseer User doesn't need it.
	*/
	Impl* GetImpl();

	/**
		@brief	デバイスロストが発生した時に実行する。
	*/
	virtual void OnLostDevice() = 0;

	/**
		@brief	デバイスがリセットされた時に実行する。
	*/
	virtual void OnResetDevice() = 0;

	/**
		@brief	このインスタンスを破棄する。
	*/
	virtual void Destroy() = 0;

	/**
		@brief	ステートを復帰するかどうかのフラグを設定する。
	*/
	virtual void SetRestorationOfStatesFlag(bool flag) = 0;

	/**
		@brief	描画を開始する時に実行する。
	*/
	virtual bool BeginRendering() = 0;

	/**
		@brief	描画を終了する時に実行する。
	*/
	virtual bool EndRendering() = 0;

	/**
		@brief	Get the direction of light
	*/
	virtual ::Effekseer::Vector3D GetLightDirection() const;

	/**
		@brief	Specifiy the direction of light
	*/
	virtual void SetLightDirection(const ::Effekseer::Vector3D& direction);

	/**
		@brief	Get the color of light
	*/
	virtual const ::Effekseer::Color& GetLightColor() const;

	/**
		@brief	Specify the color of light
	*/
	virtual void SetLightColor(const ::Effekseer::Color& color);

	/**
		@brief	Get the color of ambient
	*/
	virtual const ::Effekseer::Color& GetLightAmbientColor() const;

	/**
		@brief	Specify the color of ambient
	*/
	virtual void SetLightAmbientColor(const ::Effekseer::Color& color);

	/**
		@brief	最大描画スプライト数を取得する。
	*/
	virtual int32_t GetSquareMaxCount() const = 0;

	/**
		@brief	Get a projection matrix
	*/
	virtual ::Effekseer::Matrix44 GetProjectionMatrix() const;

	/**
		@brief	Set a projection matrix
	*/
	virtual void SetProjectionMatrix(const ::Effekseer::Matrix44& mat);

	/**
		@brief	Get a camera matrix
	*/
	virtual ::Effekseer::Matrix44 GetCameraMatrix() const;

	/**
		@brief	Set a camera matrix
	*/
	virtual void SetCameraMatrix(const ::Effekseer::Matrix44& mat);

	/**
		@brief	Get a camera projection matrix
	*/
	virtual ::Effekseer::Matrix44 GetCameraProjectionMatrix() const;

	/**
		@brief	Get a front direction of camera
		@note
		We don't recommend to use it without understanding of internal code.
	*/
	virtual ::Effekseer::Vector3D GetCameraFrontDirection() const;

	/**
		@brief	Get a position of camera
		@note
		We don't recommend to use it without understanding of internal code.
	*/
	virtual ::Effekseer::Vector3D GetCameraPosition() const;

	/**
		@brief	Set a front direction and position of camera manually
		@param front (Right Hand) a direction from focus to eye, (Left Hand) a direction from eye to focus, 
		@note
		These are set based on camera matrix automatically.
		It is failed on some platform.
	*/
	virtual void SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position);

	/**
		@brief	スプライトレンダラーを生成する。
	*/
	virtual ::Effekseer::SpriteRenderer* CreateSpriteRenderer() = 0;

	/**
		@brief	リボンレンダラーを生成する。
	*/
	virtual ::Effekseer::RibbonRenderer* CreateRibbonRenderer() = 0;

	/**
		@brief	リングレンダラーを生成する。
	*/
	virtual ::Effekseer::RingRenderer* CreateRingRenderer() = 0;

	/**
		@brief	モデルレンダラーを生成する。
	*/
	virtual ::Effekseer::ModelRenderer* CreateModelRenderer() = 0;

	/**
		@brief	軌跡レンダラーを生成する。
	*/
	virtual ::Effekseer::TrackRenderer* CreateTrackRenderer() = 0;

	/**
		@brief	標準のテクスチャ読込クラスを生成する。
	*/
	virtual ::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface = NULL) = 0;

	/**
		@brief	標準のモデル読込クラスを生成する。
	*/
	virtual ::Effekseer::ModelLoader* CreateModelLoader(::Effekseer::FileInterface* fileInterface = NULL) = 0;

	/**
	@brief	
	\~english Create default material loader
	\~japanese 標準のマテリアル読込クラスを生成する。

	*/
	virtual ::Effekseer::MaterialLoader* CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) = 0;

	/**
		@brief	レンダーステートを強制的にリセットする。
	*/
	virtual void ResetRenderState() = 0;

	/**
	@brief	背景を歪ませるエフェクトが描画される前に呼ばれるコールバックを取得する。
	*/
	virtual DistortingCallback* GetDistortingCallback() = 0;

	/**
	@brief	背景を歪ませるエフェクトが描画される前に呼ばれるコールバックを設定する。
	*/
	virtual void SetDistortingCallback(DistortingCallback* callback) = 0;

	/**
	@brief	
	\~english Get draw call count
	\~japanese ドローコールの回数を取得する
	*/
	virtual int32_t GetDrawCallCount() const;

	/**
	@brief
	\~english Get the number of vertex drawn
	\~japanese 描画された頂点数をリセットする
	*/
	virtual int32_t GetDrawVertexCount() const;

	/**
	@brief
	\~english Reset draw call count
	\~japanese ドローコールの回数をリセットする
	*/
	virtual void ResetDrawCallCount();

	/**
	@brief
	\~english Reset the number of vertex drawn
	\~japanese 描画された頂点数をリセットする
	*/
	virtual void ResetDrawVertexCount();

	/**
	@brief
	\~english Get a render mode.
	\~japanese 描画モードを取得する。
	*/
	virtual Effekseer::RenderMode GetRenderMode() const;

	/**
	@brief	
	\~english Specify a render mode.
	\~japanese 描画モードを設定する。
	*/
	virtual void SetRenderMode(Effekseer::RenderMode renderMode);

	/**
	@brief
	\~english Get an UV Style of texture when particles are rendered.
	\~japanese パーティクルを描画するときのUVの状態を取得する。
	*/
	virtual UVStyle GetTextureUVStyle() const;

	/**
	@brief
	\~english Set an UV Style of texture when particles are rendered.
	\~japanese パーティクルを描画するときのUVの状態を設定する。
	*/
	virtual void SetTextureUVStyle(UVStyle style);

	/**
	@brief
	\~english Get an UV Style of background when particles are rendered.
	\~japanese パーティクルを描画するときの背景のUVの状態を取得する。
	*/
	virtual UVStyle GetBackgroundTextureUVStyle() const;

	/**
	@brief
	\~english Set an UV Style of background when particles are rendered.
	\~japanese パーティクルを描画するときの背景のUVの状態を設定する。
	*/
	virtual void SetBackgroundTextureUVStyle(UVStyle style);

	/**
	@brief
	\~english Get a current time (s)
	\~japanese 現在の時間を取得する。(秒)
	*/
	virtual float GetTime() const;

	/**
	@brief
	\~english Set a current time (s)
	\~japanese 現在の時間を設定する。(秒)
	*/
	virtual void SetTime(float time);

	/**
	@brief
	\~English	specify a command list to render.  This function is available except DirectX9, DirectX11 and OpenGL.
	\~Japanese	描画に使用するコマンドリストを設定する。この関数はDirectX9、DirectX11、OpenGL以外で使用できる。
	*/
	virtual void SetCommandList(CommandList* commandList)
	{
	}

	/**
	@brief
	\~English	Specify a background texture.
	\~Japanese	背景のテクスチャを設定する。
	@note
	\~English	Specified texture is not deleted by the renderer. This function is available except DirectX9, DirectX11.
	\~Japanese	設定されたテクスチャはレンダラーによって削除されない。この関数はDirectX9、DirectX11以外で使用できる。
	*/
	virtual void SetBackgroundTexture(::Effekseer::TextureData* textureData);

	/**
	@brief
	\~English	Create a proxy texture
	\~Japanese	代替のテクスチャを生成する
	*/
	virtual Effekseer::TextureData* CreateProxyTexture(ProxyTextureType type)
	{
		return nullptr;
	}

	/**
	@brief
	\~English	Delete a proxy texture
	\~Japanese	代替のテクスチャを削除する
	*/
	virtual void DeleteProxyTexture(Effekseer::TextureData* data)
	{
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_RENDERER_H__

#ifndef __EFFEKSEERRENDERER_GL_RENDERER_H__
#define __EFFEKSEERRENDERER_GL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
// Lib
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{

class DeviceObjectCollection;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
@brief	テクスチャ読込クラスを生成する。
*/
::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface = nullptr, ::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);

/**
@brief	モデル読込クラスを生成する。
*/
::Effekseer::ModelLoader* CreateModelLoader(::Effekseer::FileInterface* fileInterface = NULL);

/**
	@brief	描画クラス
*/
class Renderer
	: public ::EffekseerRenderer::Renderer
{
protected:
	Renderer()
	{
	}
	virtual ~Renderer()
	{
	}

public:
	/**
	@brief
	\~english	Create an instance
	\~japanese	インスタンスを生成する。
	@param	squareMaxCount
	\~english	the number of maximum sprites
	\~japanese	最大描画スプライト数
	@param	deviceType
	\~english	device type of opengl
	\~japanese	デバイスの種類
	@param	deviceObjectCollection
	\~english	for a middleware. it should be nullptr.
	\~japanese	ミドルウェア向け。 nullptrにすべきである。
	@return
	\~english	instance
	\~japanese	インスタンス
	*/
	static Renderer* Create(int32_t squareMaxCount,
							OpenGLDeviceType deviceType = OpenGLDeviceType::OpenGL2,
							DeviceObjectCollection* deviceObjectCollection = nullptr);

	/**
		@brief	最大描画スプライト数を取得する。
	*/
	virtual int32_t GetSquareMaxCount() const = 0;

	/**
		@brief	最大描画スプライト数を設定する。
		@note
		描画している時は使用できない。
	*/
	virtual void SetSquareMaxCount(int32_t count) = 0;

	/**
	@brief
	\~english	Get a background.
	\~japanese	背景を取得する。
	*/
	virtual Effekseer::TextureData* GetBackground() = 0;

	/**
	@brief	
	\~english	Specify a background.
	\~japanese	背景を設定する。
	*/
	virtual void SetBackground(GLuint background, bool hasMipmap = false) = 0;

	/**
	@brief	
	\~english get a device type
	\~japanese デバイスの種類を取得する。
	*/
	virtual OpenGLDeviceType GetDeviceType() const = 0;

	/**
	@brief
	\~english get whether VAO is supported
	\~japanese VAOがサポートされているか取得する。
	*/
	virtual bool IsVertexArrayObjectSupported() const = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	モデル
*/
class Model
	: public Effekseer::Model
{
private:
public:
	struct InternalModel
	{
		GLuint VertexBuffer;
		GLuint IndexBuffer;
		int32_t VertexCount;
		int32_t IndexCount;

		std::vector<uint8_t> delayVertexBuffer;
		std::vector<uint8_t> delayIndexBuffer;

		InternalModel();

		virtual ~InternalModel();

		bool TryDelayLoad();
	};

	InternalModel* InternalModels = nullptr;
	int32_t ModelCount;
	bool IsLoadedOnGPU = false;

	Model(void* data, int32_t size);
	~Model();

	bool LoadToGPU();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_RENDERER_H__
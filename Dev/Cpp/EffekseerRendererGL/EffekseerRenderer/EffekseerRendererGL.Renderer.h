
#ifndef __EFFEKSEERRENDERER_GL_RENDERER_H__
#define __EFFEKSEERRENDERER_GL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererGL.Base.h"

//----------------------------------------------------------------------------------
// Lib
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(OpenGLDeviceType deviceType, bool isExtensionsEnabled = true);

[[deprecated("please use EffekseerRenderer::CreateTextureLoader")]] ::Effekseer::TextureLoaderRef CreateTextureLoader(
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
	::Effekseer::FileInterface* fileInterface = nullptr,
	::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);

[[deprecated("please use EffekseerRenderer::CreateModelLoader")]] ::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterface* fileInterface = nullptr, OpenGLDeviceType deviceType = OpenGLDeviceType::OpenGL2);

::Effekseer::MaterialLoaderRef CreateMaterialLoader(Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
													::Effekseer::FileInterface* fileInterface = nullptr);

Effekseer::Backend::TextureRef CreateTexture(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, GLuint buffer, bool hasMipmap, const std::function<void()>& onDisposed);

/**
		@brief	\~English	Properties in a texture
				\~Japanese	テクスチャ内のプロパティ
*/
struct TextureProperty
{
	GLuint Buffer = 0;
};

TextureProperty GetTextureProperty(::Effekseer::Backend::TextureRef texture);

class Renderer;
using RendererRef = ::Effekseer::RefPtr<Renderer>;

class Renderer : public ::EffekseerRenderer::Renderer
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
	@param	isExtensionsEnabled
	\~english	whether does make extensions enabled.
	\~japanese	拡張を有効にするかどうか
	@return
	\~english	instance
	\~japanese	インスタンス
	*/
	static RendererRef Create(int32_t squareMaxCount, OpenGLDeviceType deviceType = OpenGLDeviceType::OpenGL2, bool isExtensionsEnabled = true);

	static RendererRef Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t squareMaxCount);

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

} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_RENDERER_H__
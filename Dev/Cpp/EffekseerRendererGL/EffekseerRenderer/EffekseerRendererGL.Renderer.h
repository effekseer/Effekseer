﻿
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

class GraphicsDevice;

::Effekseer::Backend::GraphicsDevice* CreateGraphicsDevice(OpenGLDeviceType deviceType);

::EffekseerRenderer::GraphicsDevice* CreateDevice(OpenGLDeviceType deviceType = OpenGLDeviceType::OpenGL2);

[[deprecated("please use CreateTextureLoader with GraphicsDevice")]]
::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface = nullptr,
												::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);

::Effekseer::TextureLoader* CreateTextureLoader(
	Effekseer::Backend::GraphicsDevice* graphicsDevice,
	::Effekseer::FileInterface* fileInterface = nullptr,
	::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);

::Effekseer::ModelLoader* CreateModelLoader(::Effekseer::FileInterface* fileInterface = NULL, OpenGLDeviceType deviceType = OpenGLDeviceType::OpenGL2);

::Effekseer::MaterialLoader* CreateMaterialLoader(::EffekseerRenderer::GraphicsDevice* graphicsDevice,
												  ::Effekseer::FileInterface* fileInterface = nullptr);

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
	@param	graphicDevice
	\~english	for a middleware. it should be nullptr.
	\~japanese	ミドルウェア向け。 nullptrにすべきである。
	@return
	\~english	instance
	\~japanese	インスタンス
	*/
	static Renderer* Create(int32_t squareMaxCount, OpenGLDeviceType deviceType = OpenGLDeviceType::OpenGL2);

	static Renderer* Create(int32_t squareMaxCount, ::EffekseerRenderer::GraphicsDevice* graphicDevice);

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
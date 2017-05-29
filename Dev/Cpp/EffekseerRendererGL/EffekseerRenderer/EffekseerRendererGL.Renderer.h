
#ifndef	__EFFEKSEERRENDERER_GL_RENDERER_H__
#define	__EFFEKSEERRENDERER_GL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Base.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"

//----------------------------------------------------------------------------------
// Lib
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
@brief	テクスチャ読込クラスを生成する。
*/
::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface = NULL);

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
	Renderer() {}
	virtual ~Renderer() {}

public:
	/**
		@brief	インスタンスを生成する。
		@param	squareMaxCount		最大描画スプライト数
		@param	OpenGLDeviceType	デバイスの種類
		@return	インスタンス
	*/
	static Renderer* Create(int32_t squareMaxCount, OpenGLDeviceType deviceType = OpenGLDeviceType::OpenGL2);

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
	@brief	背景を取得する。
	*/
	virtual Effekseer::TextureData* GetBackground() = 0;

	/**
	@brief	背景を設定する。
	*/
	virtual void SetBackground(GLuint background) = 0;
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
	GLuint		VertexBuffer;
	GLuint		IndexBuffer;
	int32_t		VertexCount;
	int32_t		IndexCount;
	int32_t		ModelCount;

	Model(void* data, int32_t size);
	~Model();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GL_RENDERER_H__
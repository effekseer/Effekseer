
#ifndef __EFFEKSEER_MODELLOADER_H__
#define __EFFEKSEER_MODELLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	モデル読み込み破棄関数指定クラス
*/
class ModelLoader
{
public:
	/**
		@brief	コンストラクタ
	*/
	ModelLoader()
	{
	}

	/**
		@brief	デストラクタ
	*/
	virtual ~ModelLoader()
	{
	}

	/**
		@brief	モデルを読み込む。
		@param	path	[in]	読み込み元パス
		@return	モデルのポインタ
		@note
		モデルを読み込む。
		::Effekseer::Effect::Create実行時に使用される。
	*/
	virtual void* Load(const EFK_CHAR* path)
	{
		return NULL;
	}

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
		\~English	a pointer of loaded texture
		\~Japanese	読み込まれたモデルのポインタ
	*/
	virtual void* Load(const void* data, int32_t size)
	{
		return nullptr;
	}

	/**
		@brief	モデルを破棄する。
		@param	data	[in]	モデル
		@note
		モデルを破棄する。
		::Effekseer::Effectのインスタンスが破棄された時に使用される。
	*/
	virtual void Unload(void* data)
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
#endif // __EFFEKSEER_MODELLOADER_H__

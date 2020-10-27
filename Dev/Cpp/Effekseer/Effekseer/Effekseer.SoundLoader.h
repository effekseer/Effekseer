
#ifndef __EFFEKSEER_SOUNDLOADER_H__
#define __EFFEKSEER_SOUNDLOADER_H__

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
	@brief	サウンド読み込み破棄関数指定クラス
*/
class SoundLoader
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
	virtual void* Load(const EFK_CHAR* path)
	{
		return NULL;
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
	virtual void* Load(const void* data, int32_t size)
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
	virtual void Unload(void* source)
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
#endif // __EFFEKSEER_SOUNDLOADER_H__

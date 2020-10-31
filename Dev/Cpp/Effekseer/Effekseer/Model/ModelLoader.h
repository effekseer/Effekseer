﻿
#ifndef __EFFEKSEER_MODELLOADER_H__
#define __EFFEKSEER_MODELLOADER_H__

#include "../Effekseer.Base.h"

namespace Effekseer
{

/**
	\~English	Model loader
	\~Japanese	モデル読み込み破棄関数指定クラス
*/
class ModelLoader
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
	virtual Model* Load(const char16_t* path);

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
	virtual Effekseer::Model* Load(const void* data, int32_t size);

	/**
		@brief
		\~English	dispose a model
		\~Japanese	モデルを破棄する。
		@param	data
		\~English	a pointer of loaded a model
		\~Japanese	読み込まれたモデルのポインタ
	*/
	virtual void Unload(Model* data);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_MODELLOADER_H__

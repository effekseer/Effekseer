
#ifndef	__EFFEKSEER_LOADER_H__
#define	__EFFEKSEER_LOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	設定クラス
	@note
	EffectLoader等、全てのレンダラー、ローダーを設定することができ、Managerの代わりにエフェクト読み込み時に使用する。
*/
class Setting
{
private:
	EffectLoader*	m_effectLoader;
	TextureLoader*	m_textureLoader;
	SoundLoader*	m_soundLoader;
	ModelLoader*	m_modelLoader;

public:
	/**
		@brief	コンストラクタ
	*/
	Setting();

	/**
		@brief	デストラクタ
	*/
	virtual ~Setting();

	/**
		@brief	エフェクトローダーを取得する。
		@return	エフェクトローダー
	*/
	EffectLoader* GetEffectLoader();

	/**
		@brief	エフェクトローダーを設定する。
		@param	loader	[in]		ローダー
	*/
	void SetEffectLoader(EffectLoader* loader);

	/**
		@brief	テクスチャローダーを取得する。
		@return	テクスチャローダー
	*/
	TextureLoader* GetTextureLoader();

	/**
		@brief	テクスチャローダーを設定する。
		@param	loader	[in]		ローダー
	*/
	void SetTextureLoader(TextureLoader* loader);

	/**
		@brief	モデルローダーを取得する。
		@return	モデルローダー
	*/
	ModelLoader* GetModelLoader();

	/**
		@brief	モデルローダーを設定する。
		@param	loader	[in]		ローダー
	*/
	void SetModelLoader(ModelLoader* loader);

	/**
		@brief	サウンドローダーを取得する。
		@return	サウンドローダー
	*/
	SoundLoader* GetSoundLoader();

	/**
		@brief	サウンドローダーを設定する。
		@param	loader	[in]		ローダー
	*/
	void SetSoundLoader(SoundLoader* loader);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_LOADER_H__

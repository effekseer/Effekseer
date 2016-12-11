
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
	EffectLoader等、ファイル読み込みに関する設定することができる。
	Managerの代わりにエフェクト読み込み時に使用することで、Managerとは独立してEffectインスタンスを生成することができる。
*/
	class Setting
		: public ReferenceObject
	{
	private:
		/* 座標系 */
		CoordinateSystem		m_coordinateSystem;

		EffectLoader*	m_effectLoader;
		TextureLoader*	m_textureLoader;
		SoundLoader*	m_soundLoader;
		ModelLoader*	m_modelLoader;

	protected:
		/**
			@brief	コンストラクタ
			*/
		Setting();

		/**
			@brief	デストラクタ
			*/
		 ~Setting();
	public:

		/**
			@brief	設定インスタンスを生成する。
		*/
		static Setting* Create();

		/**
		@brief	座標系を取得する。
		@return	座標系
		*/
		CoordinateSystem GetCoordinateSystem() const;

		/**
		@brief	座標系を設定する。
		@param	coordinateSystem	[in]	座標系
		@note
		座標系を設定する。
		エフェクトファイルを読み込む前に設定する必要がある。
		*/
		void SetCoordinateSystem(CoordinateSystem coordinateSystem);

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

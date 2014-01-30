
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

		/* スプライト描画機能用インスタンス */
		SpriteRenderer*				m_spriteRenderer;

		/* リボン描画機能用インスタンス */
		RibbonRenderer*				m_ribbonRenderer;

		/* リング描画機能用インスタンス */
		RingRenderer*				m_ringRenderer;

		/* モデル描画機能用インスタンス */
		ModelRenderer*				m_modelRenderer;

		/* トラック描画機能用インスタンス */
		TrackRenderer*				m_trackRenderer;

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

		/**
		@brief	スプライト描画機能を取得する。
		*/
		SpriteRenderer* GetSpriteRenderer();

		/**
		@brief	スプライト描画機能を設定する。
		*/
		void SetSpriteRenderer(SpriteRenderer* renderer);

		/**
		@brief	ストライプ描画機能を取得する。
		*/
		RibbonRenderer* GetRibbonRenderer();

		/**
		@brief	ストライプ描画機能を設定する。
		*/
		void SetRibbonRenderer(RibbonRenderer* renderer);

		/**
		@brief	リング描画機能を取得する。
		*/
		RingRenderer* GetRingRenderer();

		/**
		@brief	リング描画機能を設定する。
		*/
		void SetRingRenderer(RingRenderer* renderer);

		/**
		@brief	モデル描画機能を取得する。
		*/
		ModelRenderer* GetModelRenderer();

		/**
		@brief	モデル描画機能を設定する。
		*/
		void SetModelRenderer(ModelRenderer* renderer);

		/**
		@brief	軌跡描画機能を取得する。
		*/
		TrackRenderer* GetTrackRenderer();

		/**
		@brief	軌跡描画機能を設定する。
		*/
		void SetTrackRenderer(TrackRenderer* renderer);

	};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_LOADER_H__

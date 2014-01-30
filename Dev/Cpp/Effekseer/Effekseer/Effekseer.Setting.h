
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

		// メモリ確保関数
		MallocFunc	m_MallocFunc;

		// メモリ破棄関数
		FreeFunc	m_FreeFunc;

		// ランダム関数
		RandFunc	m_randFunc;

		// ランダム関数最大値
		int			m_randMax;

		/* 座標系 */
		eCoordinateSystem		m_coordinateSystem;

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

		/* サウンド再生用インスタンス */
		SoundPlayer*				m_soundPlayer;

	public:
		/**
			@brief	コンストラクタ
			*/
		Setting();

		/**
			@brief	デストラクタ
			*/
		 ~Setting();

		/**
		@brief	メモリ確保関数を取得する。
		*/
		 MallocFunc GetMallocFunc() const ;

		/**
		@brief	メモリ確保関数を設定する。
		*/
		 void SetMallocFunc(MallocFunc func) ;

		/**
		@brief	メモリ破棄関数を取得する。
		*/
		 FreeFunc GetFreeFunc() const ;

		/**
		@brief	メモリ破棄関数を設定する。
		*/
		 void SetFreeFunc(FreeFunc func) ;

		/**
		@brief	ランダム関数を取得する。
		*/
		 RandFunc GetRandFunc() const ;

		/**
		@brief	ランダム関数を設定する。
		*/
		 void SetRandFunc(RandFunc func) ;

		/**
		@brief	ランダム最大値を取得する。
		*/
		 int GetRandMax() const ;

		/**
		@brief	ランダム関数を設定する。
		*/
		 void SetRandMax(int max_) ;

		/**
		@brief	座標系を取得する。
		@return	座標系
		*/
		eCoordinateSystem GetCoordinateSystem() const;

		/**
		@brief	座標系を設定する。
		@param	coordinateSystem	[in]	座標系
		@note
		座標系を設定する。
		エフェクトファイルを読み込む前に設定する必要がある。
		*/
		void SetCoordinateSystem(eCoordinateSystem coordinateSystem);

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

		/**
		@brief	サウンド再生機能を取得する。
		*/
		SoundPlayer* GetSoundPlayer();

		/**
		@brief	サウンド再生機能を設定する。
		*/
		void SetSoundPlayer(SoundPlayer* soundPlayer);

	};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_LOADER_H__

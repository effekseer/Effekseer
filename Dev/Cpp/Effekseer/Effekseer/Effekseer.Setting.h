
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
		int32_t		m_ref;

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

		// メモリ確保関数
		static void* EFK_STDCALL Malloc(unsigned int size);

		// メモリ破棄関数
		static void EFK_STDCALL Free(void* p, unsigned int size);

		// ランダム関数
		static int EFK_STDCALL Rand();

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
			@brief	参照カウンタを加算する。
			@return	参照カウンタ
		*/
		int32_t AddRef();

		/**
			@brief	参照カウンタを減算する。
			@return	参照カウンタ
		*/
		int32_t Release();

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
	};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_LOADER_H__

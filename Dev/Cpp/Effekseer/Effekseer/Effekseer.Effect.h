
#ifndef	__EFFEKSEER_EFFECT_H__
#define	__EFFEKSEER_EFFECT_H__

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
	@brief	エフェクトパラメータークラス
	@note
	エフェクトに設定されたパラメーター。
*/
class Effect
{
protected:
	Effect() {}
    ~Effect() {}

public:

	/**
		@brief	エフェクトを生成する。
		@param	manager			[in]	管理クラス
		@param	data			[in]	データ配列の先頭のポインタ
		@param	size			[in]	データ配列の長さ
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create( Manager* manager, void* data, int32_t size, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	エフェクトを生成する。
		@param	manager			[in]	管理クラス
		@param	path			[in]	読込元のパス
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create( Manager* manager, const EFK_CHAR* path, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	参照カウンタを加算する。
		@return	実行後の参照カウンタの値
	*/
	virtual int AddRef() = 0;

	/**
		@brief	参照カウンタを減算する。
		@return	実行後の参照カウンタの値
	*/
	virtual int Release() = 0;

	/**
		@brief	マネージャーを取得する。
		@return	マネージャー
	*/
	virtual Manager* GetManager() const = 0;

	/**
	@brief	設定を取得する。
	@return	設定
	*/
	virtual Setting* GetSetting() const = 0;

	/* 拡大率を取得する。 */
	virtual float GetMaginification() const = 0;
	
	/**
		@brief	エフェクトデータのバージョン取得
	*/
	virtual int GetVersion() const = 0;

	/**
		@brief	格納されている画像のポインタを取得する。
		@param	n	[in]	画像のインデックス
		@return	画像のポインタ
	*/
	virtual void* GetImage( int n ) const = 0;

	/**
		@brief	格納されている音波形のポインタを取得する。
	*/
	virtual void* GetWave( int n ) const = 0;

	/**
		@brief	格納されているモデルのポインタを取得する。
	*/
	virtual void* GetModel( int n ) const = 0;

	/**
		@brief	エフェクトのリロードを行う。
	*/
	virtual bool Reload( void* data, int32_t size, const EFK_CHAR* materialPath = NULL ) = 0;

	/**
		@brief	エフェクトのリロードを行う。
	*/
	virtual bool Reload( const EFK_CHAR* path, const EFK_CHAR* materialPath = NULL ) = 0;

	/**
		@brief	画像等リソースの再読み込みを行う。
	*/
	virtual void ReloadResources( const EFK_CHAR* materialPath = NULL ) = 0;

	/**
		@brief	画像等リソースの破棄を行う。
	*/
	virtual void UnloadResources() = 0;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECT_H__

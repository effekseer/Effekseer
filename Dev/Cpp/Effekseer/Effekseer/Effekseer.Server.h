
#ifndef	__EFFEKSEER_SERVER_H__
#define	__EFFEKSEER_SERVER_H__

#if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

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
class Server
{
public:

	Server() {}
	virtual ~Server() {}

	static Server* Create();

	/**
		@brief	サーバーを開始する。
	*/
	virtual bool Start( uint16_t port ) = 0;

	virtual void Stop() = 0;

	/**
		@brief	エフェクトをリロードの対象として登録する。
		@param	key	[in]	検索用キー
		@param	effect	[in]	リロードする対象のエフェクト
	*/
	virtual void Regist( const EFK_CHAR* key, Effect* effect ) = 0;

	/**
		@brief	エフェクトをリロードの対象から外す。
		@param	effect	[in]	リロードから外すエフェクト
	*/
	virtual void Unregist( Effect* effect ) = 0;

	/**
		@brief	サーバーを更新し、エフェクトのリロードを行う。
	*/
	virtual void Update() = 0;

	/**
		@brief	素材パスを設定する。
	*/
	virtual void SetMaterialPath( const EFK_CHAR* materialPath ) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif	// #if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

#endif	// __EFFEKSEER_SERVER_H__

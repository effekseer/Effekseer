﻿
#ifndef __EFFEKSEER_SERVER_H__
#define __EFFEKSEER_SERVER_H__

#include "../Effekseer.Base.Pre.h"

namespace Effekseer
{

class Server;
using ServerRef = RefPtr<Server>;

/**
	@brief
	\~English	A server to edit effect from client such an editor
	\~Japanese	エディタといったクライアントからエフェクトを編集するためのサーバー
*/
class Server : public IReference
{
public:
	Server()
	{
	}
	virtual ~Server()
	{
	}

	/**
		@brief
		\~English	create a server instance
		\~Japanese	サーバーのインスタンスを生成する。
	*/
	static ServerRef Create();

	/**
		@brief
		\~English	start a server
		\~Japanese	サーバーを開始する。
	*/
	virtual bool Start(uint16_t port) = 0;

	/**
		@brief
		\~English	stop a server
		\~Japanese	サーバーを終了する。
	*/
	virtual void Stop() = 0;

	/**
		@brief
		\~English	register an effect as a target to edit.
		\~Japanese	エフェクトを編集の対象として登録する。
		@param	key
		\~English	a key to search an effect
		\~Japanese	検索用キー
		@param	effect
		\~English	an effect to be edit
		\~Japanese	編集される対象のエフェクト
	*/
	virtual void Register(const char16_t* key, const EffectRef& effect) = 0;

	/**
		@brief
		\~English	unregister an effect
		\~Japanese	エフェクトを対象から外す。
		@param	effect
		\~English	an effect registered
		\~Japanese	登録されているエフェクト
	*/
	virtual void Unregister(const EffectRef& effect) = 0;

	/**
		@brief
		\~English	update a server and reload effects
		\~Japanese	サーバーを更新し、エフェクトのリロードを行う。
		@brief	managers
		\~English	all managers which is playing effects.
		\~Japanese	エフェクトを再生している全てのマネージャー
		@brief	managerCount
		\~English	the number of manager
		\~Japanese	マネージャーの個数

	*/
	virtual void
	Update(ManagerRef* managers, int32_t managerCount, ReloadingThreadType reloadingThreadType = ReloadingThreadType::Main) = 0;

	/**
		@brief
		\~English	Specify root path to load materials
		\~Japanese	素材のルートパスを設定する。
	*/
	virtual void SetMaterialPath(const char16_t* materialPath) = 0;
	
	/**
		@brief
		\~English	Check the connection status
		\~Japanese	接続状態をチェックする。
	*/
	virtual bool IsConnected() const = 0;
};

} // namespace Effekseer

#endif // __EFFEKSEER_SERVER_H__

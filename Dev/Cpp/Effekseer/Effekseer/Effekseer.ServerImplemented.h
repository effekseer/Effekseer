﻿
#ifndef __EFFEKSEER_SERVER_IMPLEMENTED_H__
#define __EFFEKSEER_SERVER_IMPLEMENTED_H__

#if (defined(__EFFEKSEER_NETWORK_ENABLED__))
#if !(defined(_PSVITA) || defined(_SWITCH) || defined(_XBOXONE))

#include "Effekseer.Base.h"
#include "Effekseer.Server.h"

#include "Effekseer.Socket.h"

#include <string>

#include <map>
#include <set>
#include <vector>

namespace Effekseer
{

class ServerImplemented : public Server, public ReferenceObject
{
private:
	class InternalClient
	{
	public:
		std::thread m_threadRecv;
		EfkSocket m_socket;
		ServerImplemented* m_server;
		bool m_active;

		std::vector<uint8_t> m_recvBuffer;

		std::vector<std::vector<uint8_t>> m_recvBuffers;
		std::mutex m_ctrlRecvBuffers;

		static void RecvAsync(void* data);

	public:
		InternalClient(EfkSocket socket_, ServerImplemented* server);
		~InternalClient();
		void ShutDown();
	};

private:
	struct EffectParameter
	{
		EffectRef EffectPtr;
		bool IsRegistered;
	};

	EfkSocket m_socket = InvalidSocket;
	uint16_t m_port = 0;

	std::thread m_thread;
	std::mutex m_ctrlClients;

	bool m_running = false;

	std::set<InternalClient*> m_clients;
	std::set<InternalClient*> m_removedClients;

	std::map<std::u16string, EffectParameter> m_effects;

	std::map<std::u16string, std::vector<uint8_t>> m_data;

	std::vector<char16_t> m_materialPath;

	void AddClient(InternalClient* client);
	void RemoveClient(InternalClient* client);
	static void AcceptAsync(void* data);

public:
	ServerImplemented();
	virtual ~ServerImplemented();

	bool Start(uint16_t port) override;

	void Stop() override;

	void Register(const char16_t* key, const EffectRef& effect) override;

	void Unregister(const EffectRef& effect) override;

	void Update(ManagerRef* managers, int32_t managerCount, ReloadingThreadType reloadingThreadType) override;

	void SetMaterialPath(const char16_t* materialPath) override;

	virtual int GetRef() override
	{
		return ReferenceObject::GetRef();
	}
	virtual int AddRef() override
	{
		return ReferenceObject::AddRef();
	}
	virtual int Release() override
	{
		return ReferenceObject::Release();
	}
};

} // namespace Effekseer

#endif // #if !( defined(_PSVITA) || defined(_SWITCH) || defined(_XBOXONE) )
#endif

#endif // __EFFEKSEER_SERVER_IMPLEMENTED_H__


#ifndef __EFFEKSEER_SERVER_IMPLEMENTED_H__
#define __EFFEKSEER_SERVER_IMPLEMENTED_H__

#if !(defined(__EFFEKSEER_NETWORK_DISABLED__))
#if !(defined(_PSVITA) || defined(_SWITCH) || defined(_XBOXONE))

#include "../Effekseer.Base.h"
#include "Effekseer.Server.h"

#include "Effekseer.Socket.h"
#include "Effekseer.Session.h"

#include <string>

#include <map>
#include <vector>

namespace Effekseer
{

class ServerImplemented : public Server, public ReferenceObject
{
private:
	struct InternalClient
	{
		Socket Socket;
		Session Session;
	};

private:
	struct EffectParameter
	{
		EffectRef EffectPtr;
		bool IsRegistered;
	};

	Socket m_socket;

	std::thread m_thread;
	std::mutex m_ctrlClients;

	bool m_running = false;

	std::vector<std::unique_ptr<InternalClient>> m_clients;
	std::map<std::u16string, EffectParameter> m_effects;
	std::u16string m_materialPath;

	struct Profiler
	{
		bool isRunning = false;
	};
	Profiler profiler_;

	struct UpdateContext
	{
		ManagerRef* managers = nullptr;
		int32_t managerCount = 0;
		ReloadingThreadType reloadingThreadType{};
	};
	UpdateContext updateContext_;


	void AcceptAsync();

	void OnReload(InternalClient& client, const Session::Request& req, Session::Response& res);

	void OnStartProfiling(InternalClient& client, const Session::Request& req, Session::Response& res);

	void OnStopProfiling(InternalClient& client, const Session::Request& req, Session::Response& res);

public:
	ServerImplemented();
	virtual ~ServerImplemented();

	bool Start(uint16_t port) override;

	void Stop() override;

	void Register(const char16_t* key, const EffectRef& effect) override;

	void Unregister(const EffectRef& effect) override;

	void Update(ManagerRef* managers, int32_t managerCount, ReloadingThreadType reloadingThreadType) override;

	void SetMaterialPath(const char16_t* materialPath) override;

	bool IsConnected() const override;

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

private:
	void UpdateProfiler();
};

} // namespace Effekseer

#endif // #if !( defined(_PSVITA) || defined(_SWITCH) || defined(_XBOXONE) )
#endif

#endif // __EFFEKSEER_SERVER_IMPLEMENTED_H__

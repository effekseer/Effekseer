
#ifndef __EFFEKSEER_SERVER_IMPLEMENTED_H__
#define __EFFEKSEER_SERVER_IMPLEMENTED_H__

#if (defined(__EFFEKSEER_NETWORK_ENABLED__))

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
		Socket socket;
		Session session;
		bool isProfiling = false;
	};

private:
	struct EffectParameter
	{
		EffectRef effect;
		bool registered;
	};

	Socket socket_;
	bool listening_ = false;

	std::thread thread_;
	std::mutex clientsMutex_;

	std::vector<std::unique_ptr<InternalClient>> clients_;
	std::map<std::u16string, EffectParameter> effects_;
	std::u16string materialPath_;

	struct UpdateContext
	{
		ManagerRef* managers = nullptr;
		int32_t managerCount = 0;
		ReloadingThreadType reloadingThreadType{};
	};
	UpdateContext updateContext_;

	void AcceptAsync();

	void OnReload(InternalClient& client, const Session::Message& msg);

	void OnStartProfiling(InternalClient& client, const Session::Message& msg);

	void OnStopProfiling(InternalClient& client, const Session::Message& msg);

	void UpdateProfiler(InternalClient& client);

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
};

} // namespace Effekseer

#endif

#endif // __EFFEKSEER_SERVER_IMPLEMENTED_H__

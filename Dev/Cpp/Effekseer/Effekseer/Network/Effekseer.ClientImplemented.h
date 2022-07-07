
#ifndef __EFFEKSEER_CLIENT_IMPLEMENTED_H__
#define __EFFEKSEER_CLIENT_IMPLEMENTED_H__

#if !(defined(__EFFEKSEER_NETWORK_DISABLED__))
#if !(defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE))

#include "../Effekseer.Base.h"
#include "Effekseer.Client.h"

#include "Effekseer.Socket.h"
#include "Effekseer.Session.h"
#include <vector>
#include <deque>

namespace Effekseer
{

class ClientImplemented : public Client, public ReferenceObject
{
private:
	Socket socket_;
	Session session_;
	std::deque<ProfileSample> receivedProfileSamples_;

public:
	ClientImplemented();
	~ClientImplemented() override;

	bool Start(const char* host, uint16_t port) override;
	void Stop() override;
	void Update() override;
	bool IsConnected() const override;

	void Reload(const char16_t* key, void* data, int32_t size) override;
	void Reload(ManagerRef manager, const char16_t* path, const char16_t* key) override;

	void StartProfiling() override;
	void StopProfiling() override;
	ProfileSample ReadProfileSample() override;
	void OnProfileSample(const Session::Message& msg);

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

#endif // #if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

#endif // __EFFEKSEER_CLIENT_IMPLEMENTED_H__
#endif
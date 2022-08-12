
#ifndef __EFFEKSEER_CLIENT_IMPLEMENTED_H__
#define __EFFEKSEER_CLIENT_IMPLEMENTED_H__

#if (defined(__EFFEKSEER_NETWORK_ENABLED__))

#include "Effekseer.Base.h"
#include "Effekseer.Client.h"

#include "Effekseer.Socket.h"
#include <set>
#include <vector>

namespace Effekseer
{

class ClientImplemented : public Client, public ReferenceObject
{
private:
	bool isThreadRunning = false;
	std::thread m_threadRecv;

	EfkSocket m_socket = InvalidSocket;
	uint16_t m_port = 0;
	std::vector<uint8_t> m_sendBuffer;

	bool m_running = false;
	std::mutex mutexStop;

	bool GetAddr(const char* host, IN_ADDR* addr);

	static void RecvAsync(void* data);
	void StopInternal();

public:
	ClientImplemented();
	~ClientImplemented() override;

	bool Start(char* host, uint16_t port);
	void Stop();

	bool Send(void* data, int32_t datasize);

	void Reload(const char16_t* key, void* data, int32_t size) override;
	void Reload(ManagerRef manager, const char16_t* path, const char16_t* key) override;

	bool IsConnected();

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
#endif
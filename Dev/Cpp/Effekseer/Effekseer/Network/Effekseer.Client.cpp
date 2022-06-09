
#if !(defined(__EFFEKSEER_NETWORK_DISABLED__))
#if !(defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE))

#include "Effekseer.Client.h"
#include "Effekseer.ClientImplemented.h"

#include "../Effekseer.EffectLoader.h"
#include "../Effekseer.Manager.h"

namespace Effekseer
{

void ClientImplemented::RecvAsync()
{
	while (true)
	{
		int32_t size = 0;
		int32_t restSize = 0;

		restSize = 4;
		while (restSize > 0)
		{
			int32_t recvSize = m_socket.Recv(&size, restSize);
			restSize -= recvSize;

			if (recvSize == 0 || recvSize == -1)
			{
				StopInternal();
				return;
			}
		}
	}
}

void ClientImplemented::StopInternal()
{
	std::lock_guard<std::mutex> lock(mutexStop);

	if (!m_running)
		return;
	m_running = false;

	m_socket.Close();

	EffekseerPrintDebug("Client : Stop(Internal)\n");
}

ClientImplemented::ClientImplemented()
{
	Socket::Initialize();
}

ClientImplemented::~ClientImplemented()
{
	Stop();

	Socket::Finalize();
}

ClientRef Client::Create()
{
	return MakeRefPtr<ClientImplemented>();
}

bool ClientImplemented::Start(const char* host, uint16_t port)
{
	if (m_running)
		return false;

	// to stop thread
	Stop();

	// connect
	bool ret = m_socket.Connect(host, port);
	if (!ret)
	{
		return false;
	}

	m_running = true;

	isThreadRunning = true;
	m_threadRecv = std::thread([this]() { RecvAsync(); });

	EffekseerPrintDebug("Client : Start\n");

	return true;
}

void ClientImplemented::Stop()
{
	StopInternal();

	if (isThreadRunning)
	{
		m_threadRecv.join();
		isThreadRunning = false;
	}

	EffekseerPrintDebug("Client : Stop\n");
}

bool ClientImplemented::Send(void* data, int32_t datasize)
{
	if (!m_running)
		return false;

	m_sendBuffer.clear();
	for (int32_t i = 0; i < sizeof(int32_t); i++)
	{
		m_sendBuffer.push_back(((uint8_t*)(&datasize))[i]);
	}

	for (int32_t i = 0; i < datasize; i++)
	{
		m_sendBuffer.push_back(((uint8_t*)(data))[i]);
	}

	int32_t size = (int32_t)m_sendBuffer.size();
	while (size > 0)
	{
		auto ret = m_socket.Send(&m_sendBuffer[m_sendBuffer.size() - size], size);
		if (ret == 0 || ret < 0)
		{
			Stop();
			return false;
		}
		size -= ret;
	}

	return true;
}

void ClientImplemented::Reload(const char16_t* key, void* data, int32_t size)
{
	int32_t keylen = 0;
	for (;; keylen++)
	{
		if (key[keylen] == 0)
			break;
	}

	std::vector<uint8_t> buf;

	for (int32_t i = 0; i < sizeof(int32_t); i++)
	{
		buf.push_back(((uint8_t*)(&keylen))[i]);
	}

	for (int32_t i = 0; i < keylen * 2; i++)
	{
		buf.push_back(((uint8_t*)(key))[i]);
	}

	for (int32_t i = 0; i < size; i++)
	{
		buf.push_back(((uint8_t*)(data))[i]);
	}

	Send(&(buf[0]), (int32_t)buf.size());
}

void ClientImplemented::Reload(ManagerRef manager, const char16_t* path, const char16_t* key)
{
	EffectLoaderRef loader = manager->GetEffectLoader();

	void* data = nullptr;
	int32_t size = 0;

	if (!loader->Load(path, data, size))
		return;

	Reload(key, data, size);

	loader->Unload(data, size);
}

bool ClientImplemented::IsConnected()
{
	return m_running;
}

} // namespace Effekseer

#endif // #if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )
#endif
﻿#include "Effekseer.Client.h"

#if (defined(__EFFEKSEER_NETWORK_ENABLED__))

#include "Effekseer.ClientImplemented.h"

#include "Effekseer.EffectLoader.h"
#include "Effekseer.Manager.h"

namespace Effekseer
{

void ClientImplemented::RecvAsync(void* data)
{
	auto client = (ClientImplemented*)data;

	while (true)
	{
		int32_t size = 0;
		int32_t restSize = 0;

		restSize = 4;
		while (restSize > 0)
		{
			auto recvSize = ::recv(client->m_socket, (char*)(&size), restSize, 0);
			restSize -= recvSize;

			if (recvSize == 0 || recvSize == -1)
			{
				client->StopInternal();
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

	Socket::Shutsown(m_socket);
	Socket::Close(m_socket);

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

bool ClientImplemented::GetAddr(const char* host, IN_ADDR* addr)
{
	HOSTENT* hostEntry = nullptr;

	// check ip adress or DNS
	addr->s_addr = ::inet_addr(host);
	if (addr->s_addr == InaddrNone)
	{
		// DNS
		hostEntry = ::gethostbyname(host);
		if (hostEntry == nullptr)
		{
			return false;
		}

		addr->s_addr = *(unsigned int*)hostEntry->h_addr_list[0];
	}

	return true;
}

bool ClientImplemented::Start(char* host, uint16_t port)
{
	if (m_running)
		return false;

	// to stop thread
	Stop();

	SOCKADDR_IN sockAddr;

	// create a socket
	EfkSocket socket_ = Socket::GenSocket();
	if (socket_ == InvalidSocket)
	{
		return false;
	}

	// get adder
	IN_ADDR addr;
	if (!GetAddr(host, &addr))
	{
		if (socket_ != InvalidSocket)
			Socket::Close(socket_);
		return false;
	}

	// generate data to connect
	memset(&sockAddr, 0, sizeof(SOCKADDR_IN));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr = addr;

	// connect
	int32_t ret = ::connect(socket_, (SOCKADDR*)(&sockAddr), sizeof(SOCKADDR_IN));
	if (ret == SocketError)
	{
		if (socket_ != InvalidSocket)
			Socket::Close(socket_);
		return false;
	}

	m_socket = socket_;
	m_port = port;

	m_running = true;

	isThreadRunning = true;
	m_threadRecv = std::thread([this]() { RecvAsync(this); });

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
		auto ret = ::send(m_socket, (const char*)(&(m_sendBuffer[m_sendBuffer.size() - size])), size, 0);
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

#else

namespace Effekseer
{
ClientRef Client::Create()
{
	return nullptr;
}
}

#endif

#if !(defined(__EFFEKSEER_NETWORK_DISABLED__))
#if !(defined(_PSVITA) || defined(_XBOXONE))

#include "Effekseer.Server.h"
#include "../Effekseer.Effect.h"
#include "Effekseer.ServerImplemented.h"
#include <string.h>

namespace Effekseer
{

ServerImplemented::ServerImplemented()
{
}

ServerImplemented::~ServerImplemented()
{
	Stop();
}

ServerRef Server::Create()
{
	return MakeRefPtr<ServerImplemented>();
}

void ServerImplemented::AcceptAsync()
{
	while (true)
	{
		Socket socket = m_socket.Accept();

		if (!socket.IsValid())
		{
			break;
		}

		// Accept and add an internal client
		std::unique_ptr<InternalClient> client(new InternalClient());
		client->Socket = std::move(socket);
		client->Session.Open(&client->Socket);
		client->Session.OnRequested(1, [this](const Session::Request& req, Session::Response& res){ OnDataReceived(req, res); });
		
		std::lock_guard<std::mutex> lock(m_ctrlClients);
		m_clients.emplace_back(std::move(client));
		
		EffekseerPrintDebug("Server : AcceptClient\n");
	}
}

void ServerImplemented::OnDataReceived(const Session::Request& req, Session::Response& res)
{
	auto& buf = req.payload;
	size_t offset = 0;

	// Extract a key string length
	int32_t keylen = 0;
	memcpy(&keylen, &buf[offset], sizeof(int32_t));
	offset += sizeof(int32_t);

	// Extract a key string
	const char16_t* keyptr = (const char16_t*)&buf[offset];
	std::u16string key(keyptr, keyptr + keylen);
	offset += keylen * sizeof(char16_t);
	
	auto it = m_effects.find(key);
	if (it != m_effects.end())
	{
		auto effect = it->second.EffectPtr;

		// Extract a effect data
		const uint8_t* effectData = &buf[offset];
		size_t effectSize = buf.size() - offset;

		// Reload the effect
		const char16_t* materialPath = (m_materialPath.size() > 0) ? m_materialPath.c_str() : nullptr;
		effect->Reload(m_managers, m_managerCount, effectData, (int32_t)effectSize, materialPath, m_reloadingThreadType);
	}
}

bool ServerImplemented::Start(uint16_t port)
{
	if (m_running)
	{
		Stop();
	}

	// Listen the port
	if (!m_socket.Listen(port, 30))
	{
		return false;
	}

	// Startup a socket accepting thread
	m_running = true;
	m_thread = std::thread([this]() { AcceptAsync(); });

	EffekseerPrintDebug("Server : Start\n");

	return true;
}

void ServerImplemented::Stop()
{
	if (!m_running)
	{
		return;
	}

	m_running = false;
	m_socket.Close();

	// Stop the accepting thread
	if (m_thread.joinable())
	{
		m_thread.join();
	}

	std::lock_guard<std::mutex> lock(m_ctrlClients);

	// Stop all clients
	m_clients.clear();
}

void ServerImplemented::Register(const char16_t* key, const EffectRef& effect)
{
	if (effect == nullptr)
	{
		return;
	}

	m_effects[key] = { effect, false };
}

void ServerImplemented::Unregister(const EffectRef& effect)
{
	if (effect == nullptr)
	{
		return;
	}

	auto it = m_effects.begin();
	auto it_end = m_effects.end();

	while (it != it_end)
	{
		if ((*it).second.EffectPtr == effect)
		{
			m_effects.erase(it);
			return;
		}

		it++;
	}
}

void ServerImplemented::Update(ManagerRef* managers, int32_t managerCount, ReloadingThreadType reloadingThreadType)
{
	m_managers = managers;
	m_managerCount = managerCount;
	m_reloadingThreadType = reloadingThreadType;

	std::lock_guard<std::mutex> lock(m_ctrlClients);

	for (auto& client : m_clients)
	{
		client->Session.Update();
	}

	auto removeIt = std::remove_if(m_clients.begin(), m_clients.end(), 
		[](const std::unique_ptr<InternalClient>& client){ return !client->Session.IsActive(); });
	m_clients.erase(removeIt, m_clients.end());
}

void ServerImplemented::SetMaterialPath(const char16_t* materialPath)
{
	m_materialPath = materialPath;
}

bool ServerImplemented::IsConnected() const
{
	return m_clients.size() > 0;
}

} // namespace Effekseer

#endif // #if !( defined(_PSVITA) || defined(_XBOXONE) )
#endif

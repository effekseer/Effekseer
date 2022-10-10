
#if (defined(__EFFEKSEER_NETWORK_ENABLED__))

#include "Effekseer.Server.h"
#include "Effekseer.ServerImplemented.h"

#include "../Effekseer.Effect.h"
#include "data/reload_generated.h"

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
		Socket socket = socket_.Accept();

		if (!socket.IsValid())
		{
			break;
		}

		// Accept and add an internal client
		std::unique_ptr<InternalClient> client(new InternalClient());
		client->socket = std::move(socket);
		client->session.Open(&client->socket);
		client->session.OnReceived(1, [this](const Session::Message& msg){ OnDataReceived(msg); });
		
		std::lock_guard<std::mutex> lock(clientsMutex_);
		clients_.emplace_back(std::move(client));
		
		EffekseerPrintDebug("Server : AcceptClient\n");
	}
}

void ServerImplemented::OnDataReceived(const Session::Message& msg)
{
	auto fb = Data::GetNetworkReload(msg.payload.data);
	auto fbKey = fb->key();
	auto fbData = fb->data();

	std::u16string key((const char16_t*)fbKey->data(), fbKey->size());
	
	auto it = effects_.find(key);
	if (it != effects_.end())
	{
		auto& effect = it->second.effect;

		const uint8_t* effectData = fbData->data();
		size_t effectSize = fbData->size();

		// Reload the effect
		const char16_t* materialPath = (materialPath_.size() > 0) ? materialPath_.c_str() : nullptr;
		effect->Reload(context_.managers, context_.managerCount, effectData, (int32_t)effectSize, materialPath, context_.reloadingThreadType);
	}
}

bool ServerImplemented::Start(uint16_t port)
{
	if (listening_)
	{
		Stop();
	}

	// Listen the port
	if (!socket_.Listen(port, 30))
	{
		return false;
	}

	// Startup a socket accepting thread
	listening_ = true;
	thread_ = std::thread([this]() { AcceptAsync(); });

	EffekseerPrintDebug("Server : Start\n");

	return true;
}

void ServerImplemented::Stop()
{
	if (!listening_)
	{
		return;
	}

	listening_ = false;
	socket_.Close();

	// Stop the accepting thread
	if (thread_.joinable())
	{
		thread_.join();
	}

	std::lock_guard<std::mutex> lock(clientsMutex_);

	// Stop all clients
	clients_.clear();
}

void ServerImplemented::Register(const char16_t* key, const EffectRef& effect)
{
	if (effect == nullptr)
	{
		return;
	}

	effects_[key] = { effect, false };
}

void ServerImplemented::Unregister(const EffectRef& effect)
{
	if (effect == nullptr)
	{
		return;
	}

	auto it = effects_.begin();
	auto it_end = effects_.end();

	while (it != it_end)
	{
		if ((*it).second.effect == effect)
		{
			effects_.erase(it);
			return;
		}

		it++;
	}
}

void ServerImplemented::Update(ManagerRef* managers, int32_t managerCount, ReloadingThreadType reloadingThreadType)
{
	context_.managers = managers;
	context_.managerCount = managerCount;
	context_.reloadingThreadType = reloadingThreadType;

	std::lock_guard<std::mutex> lock(clientsMutex_);

	for (auto& client : clients_)
	{
		client->session.Update();
	}

	auto removeIt = std::remove_if(clients_.begin(), clients_.end(), 
		[](const std::unique_ptr<InternalClient>& client){ return !client->session.IsActive(); });
	clients_.erase(removeIt, clients_.end());
}

void ServerImplemented::SetMaterialPath(const char16_t* materialPath)
{
	materialPath_ = materialPath;
}

bool ServerImplemented::IsConnected() const
{
	return clients_.size() > 0;
}

} // namespace Effekseer

#endif

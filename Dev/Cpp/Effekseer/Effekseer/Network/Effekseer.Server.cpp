
#if !(defined(__EFFEKSEER_NETWORK_DISABLED__))
#if !(defined(_PSVITA) || defined(_XBOXONE))

#include "Effekseer.Server.h"
#include "../Effekseer.Effect.h"
#include "../Effekseer.ManagerImplemented.h"
#include "Effekseer.ServerImplemented.h"
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
		auto clientPtr = client.get();

		client->Socket = std::move(socket);
		client->Session.Open(&client->Socket);
		client->Session.OnRequested(1, [this, clientPtr](const Session::Request& req, Session::Response& res){
			OnReload(*clientPtr, req, res);
		});
		client->Session.OnRequested(100, [this, clientPtr](const Session::Request& req, Session::Response& res){
			OnStartProfiling(*clientPtr, req, res);
		});
		client->Session.OnRequested(101, [this, clientPtr](const Session::Request& req, Session::Response& res){
			OnStopProfiling(*clientPtr, req, res);
		});
		
		std::lock_guard<std::mutex> lock(clientsMutex_);
		clients_.emplace_back(std::move(client));
		
		EffekseerPrintDebug("Server : AcceptClient\n");
	}
}

void ServerImplemented::OnReload(InternalClient& client, const Session::Request& req, Session::Response& res)
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
		const char16_t* materialPath = (m_materialPath.size() > 0) ? m_materialPath.c_str() : nullptr;
		effect->Reload(updateContext_.managers, updateContext_.managerCount, effectData, (int32_t)effectSize, materialPath, updateContext_.reloadingThreadType);
	}
}

void ServerImplemented::OnStartProfiling(InternalClient& client, const Session::Request& req, Session::Response& res)
{
	profiler_.isRunning = true;
}

void ServerImplemented::OnStopProfiling(InternalClient& client, const Session::Request& req, Session::Response& res)
{
	profiler_.isRunning = false;
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
	updateContext_.managers = managers;
	updateContext_.managerCount = managerCount;
	updateContext_.reloadingThreadType = reloadingThreadType;

	std::lock_guard<std::mutex> lock(clientsMutex_);

	for (auto& client : clients_)
	{
		client->session.Update();
	}

	auto removeIt = std::remove_if(m_clients.begin(), m_clients.end(), 
		[](const std::unique_ptr<InternalClient>& client){ return !client->Session.IsActive(); });
	m_clients.erase(removeIt, m_clients.end());

	if (profiler_.isRunning)
	{
		UpdateProfiler();
	}
}

void ServerImplemented::UpdateProfiler()
{
	for (int32_t i = 0; i < updateContext_.managerCount; i++)
	{
		auto manager = updateContext_.managers[i]->GetImplemented();
		auto& drawSets = manager->GetPlayingDrawSets();
		for (auto& handleAndDrawSet : drawSets)
		{
			for (auto& keyAndEffect : m_effects)
			{
				if (handleAndDrawSet.second.ParameterPointer == keyAndEffect.second.EffectPtr)
				{
					break;
				}
			}
		}
	}
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

#endif // #if !( defined(_PSVITA) || defined(_XBOXONE) )
#endif

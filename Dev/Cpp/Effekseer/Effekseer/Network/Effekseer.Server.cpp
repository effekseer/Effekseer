
#include "Effekseer.Server.h"
#include "Effekseer.ServerImplemented.h"

#if (defined(__EFFEKSEER_NETWORK_ENABLED__))
#include "../Effekseer.Effect.h"
#include "../Effekseer.ManagerImplemented.h"
#include "Effekseer.ServerImplemented.h"
#include "data/profiler_generated.h"
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

		client->socket = std::move(socket);
		client->session.Open(&client->socket);
		client->session.OnReceived(1, [this, clientPtr](const Session::Message& msg)
								   { OnReload(*clientPtr, msg); });
		client->session.OnReceived(100, [this, clientPtr](const Session::Message& msg)
								   { OnStartProfiling(*clientPtr, msg); });
		client->session.OnReceived(101, [this, clientPtr](const Session::Message& msg)
								   { OnStopProfiling(*clientPtr, msg); });

		std::lock_guard<std::mutex> lock(clientsMutex_);
		clients_.emplace_back(std::move(client));

		EffekseerPrintDebug("Server : AcceptClient\n");
	}
}

void ServerImplemented::OnReload(InternalClient& client, const Session::Message& msg)
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
		effect->Reload(updateContext_.managers, updateContext_.managerCount, effectData, (int32_t)effectSize, materialPath, updateContext_.reloadingThreadType);
	}
}

void ServerImplemented::OnStartProfiling(InternalClient& client, const Session::Message& msg)
{
	client.isProfiling = true;
}

void ServerImplemented::OnStopProfiling(InternalClient& client, const Session::Message& msg)
{
	client.isProfiling = false;
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
	thread_ = std::thread([this]()
						  { AcceptAsync(); });

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

	effects_[key] = {effect, false};
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
		UpdateProfiler(*client.get());
		client->session.Update();
	}

	auto removeIt = std::remove_if(clients_.begin(), clients_.end(), [](const std::unique_ptr<InternalClient>& client)
								   { return !client->session.IsActive(); });
	clients_.erase(removeIt, clients_.end());
}

void ServerImplemented::UpdateProfiler(InternalClient& client)
{
	if (!client.isProfiling)
	{
		return;
	}

	Data::flatbuffers::FlatBufferBuilder fbb;

	std::vector<Data::flatbuffers::Offset<Data::NetworkManagerProfile>> fbManagers;
	std::vector<Data::flatbuffers::Offset<Data::NetworkEffectProfile>> fbEffects;

	struct EffectProfile
	{
		const char16_t* key = nullptr;
		EffectRef effect;
		uint32_t handleCount = 0;
		float gpuTime = 0.0f;
	};

	std::vector<EffectProfile> effectProfiles;
	for (auto& keyAndEffect : effects_)
	{
		effectProfiles.emplace_back(EffectProfile{keyAndEffect.first.c_str(), keyAndEffect.second.effect});
	}

	for (int32_t i = 0; i < updateContext_.managerCount; i++)
	{
		auto manager = updateContext_.managers[i]->GetImplemented();
		auto& drawSets = manager->GetPlayingDrawSets();

		for (auto& handleAndDrawSet : drawSets)
		{
			for (auto& profile : effectProfiles)
			{
				if (handleAndDrawSet.second.ParameterPointer == profile.effect)
				{
					profile.handleCount += 1;
					profile.gpuTime += manager->GetGPUTime(handleAndDrawSet.first);
					break;
				}
			}
		}

		fbManagers.emplace_back(Data::CreateNetworkManagerProfile(fbb,
																  (uint32_t)drawSets.size(),
																  manager->GetUpdateTime() + manager->GetDrawTime(),
																  manager->GetGPUTime()));
	}

	for (auto& profile : effectProfiles)
	{
		fbEffects.emplace_back(Data::CreateNetworkEffectProfile(fbb,
																fbb.CreateVector((const uint16_t*)profile.key, std::char_traits<char16_t>::length(profile.key)),
																profile.handleCount,
																profile.gpuTime));
	}

	auto fbRoot = Data::CreateNetworkProfileSample(fbb,
												   fbb.CreateVector(fbManagers),
												   fbb.CreateVector(fbEffects));

	fbb.Finish(fbRoot);
	auto fbBufferSpan = fbb.GetBufferSpan();
	client.session.Send(102, {fbBufferSpan.data(), fbBufferSpan.size()});
}

void ServerImplemented::SetMaterialPath(const char16_t* materialPath)
{
	materialPath_ = materialPath;
}

bool ServerImplemented::IsConnected() const
{
	return clients_.size() > 0;
}

ServerRef Server::Create()
{
	return MakeRefPtr<ServerImplemented>();
}

} // namespace Effekseer

#else

namespace Effekseer
{
ServerRef Server::Create()
{
	return nullptr;
}
} // namespace Effekseer

#endif

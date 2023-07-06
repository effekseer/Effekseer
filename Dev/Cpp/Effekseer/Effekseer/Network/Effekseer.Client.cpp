

#include "Effekseer.Client.h"
#include "Effekseer.ClientImplemented.h"

#if (defined(__EFFEKSEER_NETWORK_ENABLED__))
#include "../Effekseer.EffectLoader.h"
#include "../Effekseer.Manager.h"
#include "data/profiler_generated.h"
#include "data/reload_generated.h"

namespace Effekseer
{

ClientImplemented::ClientImplemented()
{
}

ClientImplemented::~ClientImplemented()
{
	Stop();
}

bool ClientImplemented::Start(const char* host, uint16_t port)
{
	// To stop thread
	Stop();

	// Connect to server
	bool ret = socket_.Connect(host, port);
	if (!ret)
	{
		return false;
	}

	session_.Open(&socket_);

	EffekseerPrintDebug("Client : Start\n");

	return true;
}

void ClientImplemented::Stop()
{
	session_.Close();

	EffekseerPrintDebug("Client : Stop\n");
}

void ClientImplemented::Update()
{
	if (session_.IsActive())
	{
		session_.Update();
	}
}

bool ClientImplemented::IsConnected() const
{
	return session_.IsActive();
}

void ClientImplemented::Reload(const char16_t* key, void* data, int32_t size)
{
	Data::flatbuffers::FlatBufferBuilder fbb;
	auto fbKey = fbb.CreateVector((const uint16_t*)key, std::char_traits<char16_t>::length(key));
	auto fbData = fbb.CreateVector((const uint8_t*)data, size);
	fbb.Finish(Data::CreateNetworkReload(fbb, fbKey, fbData));
	auto fbBuffer = fbb.GetBufferSpan();

	session_.Send(1, {fbBuffer.data(), fbBuffer.size()});
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

void ClientImplemented::StartProfiling()
{
	session_.Send(100, Session::ByteData{nullptr, 0});
	session_.OnReceived(102, [this](const Session::Message& msg)
						{ OnProfileSample(msg); });
}

void ClientImplemented::StopProfiling()
{
	session_.Send(101, Session::ByteData{nullptr, 0});
}

Client::ProfileSample ClientImplemented::ReadProfileSample()
{
	if (receivedProfileSamples_.size() > 0)
	{
		ProfileSample profileSample(std::move(receivedProfileSamples_.front()));
		receivedProfileSamples_.pop_front();
		return std::move(profileSample);
	}
	return ProfileSample();
}

void ClientImplemented::OnProfileSample(const Session::Message& msg)
{
	auto fb = Data::GetNetworkProfileSample(msg.payload.data);

	ProfileSample profileSample;
	profileSample.IsValid = true;

	for (auto fbManager : *fb->managers())
	{
		ProfileSample::Manager profileManager;
		profileManager.CPUTime = fbManager->cpu_time();
		profileManager.GPUTime = fbManager->gpu_time();
		profileManager.HandleCount = fbManager->handle_count();
		profileSample.Managers.emplace_back(profileManager);
	}

	for (auto fbEffect : *fb->effects())
	{
		ProfileSample::Effect profileEffect;
		profileEffect.Key.assign((const char16_t*)fbEffect->key()->data(), (size_t)fbEffect->key()->size());
		profileEffect.GPUTime = fbEffect->gpu_time();
		profileEffect.HandleCount = fbEffect->handle_count();
		profileSample.Effects.emplace_back(profileEffect);
	}

	receivedProfileSamples_.emplace_back(std::move(profileSample));
}

ClientRef Client::Create()
{
	return MakeRefPtr<ClientImplemented>();
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

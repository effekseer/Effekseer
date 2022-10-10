
#if (defined(__EFFEKSEER_NETWORK_ENABLED__))

#include "Effekseer.Client.h"
#include "Effekseer.ClientImplemented.h"

#include "../Effekseer.EffectLoader.h"
#include "../Effekseer.Manager.h"
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

ClientRef Client::Create()
{
	return MakeRefPtr<ClientImplemented>();
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

void ClientImplemented::Reload(const char16_t* key, void* data, int32_t size)
{
	Data::flatbuffers::FlatBufferBuilder fbb;
	auto fbKey = fbb.CreateVector((const uint16_t*)key, std::char_traits<char16_t>::length(key));
	auto fbData = fbb.CreateVector((const uint8_t*)data, size);
	fbb.Finish(Data::CreateNetworkReload(fbb, fbKey, fbData));
	auto fbBuffer = fbb.GetBufferSpan();

	session_.Send(1, { fbBuffer.data(), fbBuffer.size() });
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

bool ClientImplemented::IsConnected() const
{
	return session_.IsActive();
}

} // namespace Effekseer

#endif
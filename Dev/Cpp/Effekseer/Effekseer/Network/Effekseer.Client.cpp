
#if !(defined(__EFFEKSEER_NETWORK_DISABLED__))
#if !(defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE))

#include "Effekseer.Client.h"
#include "Effekseer.ClientImplemented.h"

#include "../Effekseer.EffectLoader.h"
#include "../Effekseer.Manager.h"

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
	// to stop thread
	Stop();

	// connect
	bool ret = m_socket.Connect(host, port);
	if (!ret)
	{
		return false;
	}

	m_session.Open(&m_socket);

	EffekseerPrintDebug("Client : Start\n");

	return true;
}

void ClientImplemented::Stop()
{
	m_session.Close();

	EffekseerPrintDebug("Client : Stop\n");
}

void ClientImplemented::Update()
{
	if (m_session.IsActive())
	{
		m_session.Update();
	}
}

bool ClientImplemented::IsConnected() const
{
	return m_session.IsActive();
}

void ClientImplemented::Reload(const char16_t* key, void* data, int32_t size)
{
	int32_t keylen = 0;
	for (;; keylen++)
	{
		if (key[keylen] == 0)
			break;
	}
	
	Session::Request request;

	request.payload.insert(request.payload.end(), (uint8_t*)(&keylen), (uint8_t*)(&keylen) + sizeof(int32_t));

	request.payload.insert(request.payload.end(), (uint8_t*)(key), (uint8_t*)(key) + keylen * 2);

	request.payload.insert(request.payload.end(), (uint8_t*)(data), (uint8_t*)(data) + size);

	m_session.SendRequest(1, request, [](const Session::Response& res){
		EffekseerPrintDebug("Client : Respond\n");
	});
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
}

void ClientImplemented::StopProfiling()
{
}

Client::ProfileSample ClientImplemented::ReadProfileSample()
{
	return ProfileSample();
}

} // namespace Effekseer

#endif // #if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )
#endif
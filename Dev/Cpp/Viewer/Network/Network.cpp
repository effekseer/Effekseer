#include "Network.h"

namespace Effekseer::Tool
{

NetworkClient::NetworkClient()
{
	client_ = Effekseer::Client::Create();
}

bool NetworkClient::StartNetwork(const char* host, uint16_t port)
{
	return client_->Start((char*)host, port);
}

void NetworkClient::StopNetwork()
{
	client_->Stop();
}

bool NetworkClient::IsConnectingNetwork()
{
	return client_->IsConnected();
}

void NetworkClient::SendDataByNetwork(const char16_t* key, void* data, int size, const char16_t* path)
{
	client_->Reload((const char16_t*)key, data, size);
}

} // namespace Effekseer::Tool
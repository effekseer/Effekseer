#pragma once

#include <Effekseer.h>

namespace Effekseer::Tool
{

class NetworkClient
{
private:
	::Effekseer::ClientRef client_ = nullptr;

public:
	NetworkClient();

	bool StartNetwork(const char* host, uint16_t port);

	void StopNetwork();

	bool IsConnectingNetwork();

	void SendDataByNetwork(const char16_t* key, void* data, int size, const char16_t* path);
};

} // namespace Effekseer::Tool

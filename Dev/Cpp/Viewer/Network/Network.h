#pragma once

#include <Effekseer.h>
#include "ProfileSample.h"

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

	void UpdateNetwork();

	bool IsConnectingNetwork();

	void SendDataByNetwork(const char16_t* key, void* data, int size, const char16_t* path);

	void StartProfiling();

	void StopProfiling();

	ProfileSample ReadProfileSample();
};

} // namespace Effekseer::Tool

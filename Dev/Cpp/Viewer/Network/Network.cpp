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

void NetworkClient::UpdateNetwork()
{
	client_->Update();
}

bool NetworkClient::IsConnectingNetwork()
{
	return client_->IsConnected();
}

void NetworkClient::SendDataByNetwork(const char16_t* key, void* data, int size, const char16_t* path)
{
	client_->Reload((const char16_t*)key, data, size);
}

void NetworkClient::StartProfiling()
{
	client_->StartProfiling();
}

void NetworkClient::StopProfiling()
{
	client_->StopProfiling();
}

ProfileSample NetworkClient::ReadProfileSample()
{
	auto profileSample = client_->ReadProfileSample();

	ProfileSample result;
	result.IsValid = profileSample.IsValid;

	for (auto& profileManager : profileSample.Managers)
	{
		ProfileSample::Manager resultManager;
		resultManager.CPUTime = profileManager.CPUTime;
		resultManager.GPUTime = profileManager.GPUTime;
		resultManager.HandleCount = profileManager.HandleCount;
		result.Managers.emplace_back(resultManager);
	}
	
	for (auto& profileEffect : profileSample.Effects)
	{
		ProfileSample::Effect resultEffect;
		resultEffect.Key = profileEffect.Key;
		resultEffect.GPUTime = profileEffect.GPUTime;
		resultEffect.HandleCount = profileEffect.HandleCount;
		result.Effects.emplace_back(resultEffect);
	}

	return result;
}

} // namespace Effekseer::Tool
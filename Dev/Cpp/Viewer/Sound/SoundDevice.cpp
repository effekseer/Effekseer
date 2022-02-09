#include "SoundDevice.h"

namespace Effekseer::Tool
{

SoundDevice ::~SoundDevice()
{
	if (sound_ != nullptr)
	{
		sound_.Reset();
	}

	if (manager_ != nullptr)
	{
		manager_->Release();
	}
}

void SoundDevice::SetListener(const Effekseer::Tool::Vector3F& pos, const Effekseer::Tool::Vector3F& at, const Effekseer::Tool::Vector3F& up)
{
	sound_->SetListener(pos, at, up);
}

std::shared_ptr<SoundDevice> SoundDevice::Create()
{
	auto manager = osm::Manager::Create();
	if (manager == nullptr)
	{
		return nullptr;
	}

	if (!manager->Initialize())
	{
		manager->Release();
		return nullptr;
	}

	auto sound = EffekseerSound::Sound::Create(manager);
	if (sound == nullptr)
	{
		manager->Release();
		return nullptr;
	}

	// EffekseeSound doesn't increament ref counter
	manager->AddRef();

	auto device = std::make_shared<SoundDevice>();
	device->manager_ = manager;
	device->sound_ = sound;

	return device;
}

} // namespace Effekseer::Tool
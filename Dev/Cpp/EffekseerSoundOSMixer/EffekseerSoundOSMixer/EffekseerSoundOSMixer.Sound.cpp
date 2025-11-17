
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../EffekseerSoundOSMixer.h"
#include "EffekseerSoundOSMixer.SoundImplemented.h"
#include "EffekseerSoundOSMixer.SoundLoader.h"
#include "EffekseerSoundOSMixer.SoundPlayer.h"

namespace EffekseerSound
{

SoundRef Sound::Create(osm::Manager* soundManager)
{
	auto sound = Effekseer::MakeRefPtr<SoundImplemented>();
	if (sound->Initialize(soundManager))
	{
		return sound;
	}
	return nullptr;
}

SoundImplemented::SoundImplemented() = default;

SoundImplemented::~SoundImplemented()
{
	StopAll();
}

bool SoundImplemented::Initialize(osm::Manager* soundManager)
{
	manager_ = soundManager;

	return true;
}

void SoundImplemented::SetListener(const ::Effekseer::Vector3D& pos, const ::Effekseer::Vector3D& at, const ::Effekseer::Vector3D& up)
{
	using Vector3D = ::Effekseer::Vector3D;

	listener_.position_ = pos;
	Vector3D::Sub(listener_.forwardVector_, at, pos);
	Vector3D::Normal(listener_.forwardVector_, listener_.forwardVector_);
	Vector3D::Normal(listener_.upVector_, up);
	Vector3D::Cross(listener_.rightVector_, listener_.forwardVector_, listener_.upVector_);
}

void SoundImplemented::Destroy()
{
	Release();
}

void SoundImplemented::Update()
{
	for (auto it = instances_.begin(); it != instances_.end();)
	{
		if (!manager_->IsPlaying(it->id_))
		{
			it = instances_.erase(it);
		}
		else
		{
			it++;
		}
	}
}

::Effekseer::SoundPlayerRef SoundImplemented::CreateSoundPlayer()
{
	return ::Effekseer::MakeRefPtr<SoundPlayer>(SoundImplementedRef::FromPinned(this));
}

::Effekseer::SoundLoaderRef SoundImplemented::CreateSoundLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::Effekseer::MakeRefPtr<SoundLoader>(SoundImplementedRef::FromPinned(this), fileInterface);
}

void SoundImplemented::StopAll()
{
	manager_->StopAll();
}

void SoundImplemented::SetMute(bool mute)
{
	mute_ = mute;
}

void SoundImplemented::AddInstance(const Instance& instance)
{
	instances_.push_back(instance);
}

void SoundImplemented::StopTag(::Effekseer::SoundTag tag)
{
	for (auto it = instances_.begin(); it != instances_.end(); it++)
	{
		if (it->tag_ == tag)
		{
			manager_->Stop(it->id_);
		}
	}
}

void SoundImplemented::PauseTag(::Effekseer::SoundTag tag, bool pause)
{
	for (auto it = instances_.begin(); it != instances_.end(); it++)
	{
		if (it->tag_ == tag)
		{
			if (pause)
			{
				manager_->Pause(it->id_);
			}
			else
			{
				manager_->Resume(it->id_);
			}
		}
	}
}

bool SoundImplemented::CheckPlayingTag(::Effekseer::SoundTag tag)
{
	bool isPlaying = false;
	for (auto it = instances_.begin(); it != instances_.end(); it++)
	{
		if (it->tag_ == tag)
		{
			isPlaying |= manager_->IsPlaying(it->id_);
		}
	}
	return isPlaying;
}

void SoundImplemented::StopData(const ::Effekseer::SoundDataRef& soundData)
{
	for (auto it = instances_.begin(); it != instances_.end(); it++)
	{
		if (it->data_ == soundData)
		{
			manager_->Stop(it->id_);
		}
	}
}

void SoundImplemented::Calculate3DSound(const ::Effekseer::Vector3D& position, float rolloffDistance, float& rolloff, float& pan)
{
	using Vector3D = ::Effekseer::Vector3D;

	Vector3D diff = position - listener_.position_;
	float distance = Vector3D::Length(diff);

	if (distance == 0.0f)
	{
		rolloff = 1.0f;
		pan = 0.0f;
		return;
	}

	// Calculate rolloff
	if (distance <= rolloffDistance)
	{
		rolloff = 1.0f;
	}
	else if (distance >= rolloffDistance)
	{
		rolloff = 0.0f;
	}
	else
	{
		rolloff = (1.0f - (distance - rolloffDistance) / rolloffDistance);
	}

	// Calculate pan
	Vector3D relativeVector = diff * (1.0f / distance);
	pan = Vector3D::Dot(relativeVector, listener_.rightVector_);
}

} // namespace EffekseerSound

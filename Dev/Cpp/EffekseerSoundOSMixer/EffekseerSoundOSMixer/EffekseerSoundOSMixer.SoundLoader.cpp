
#include "EffekseerSoundOSMixer.SoundLoader.h"
#include "EffekseerSoundOSMixer.SoundImplemented.h"
#include <assert.h>
#include <memory>
#include <string.h>

namespace EffekseerSound
{

SoundLoader::SoundLoader(const SoundImplementedRef& sound, ::Effekseer::FileInterfaceRef fileInterface)
	: sound_(sound)
	, fileInterface_(fileInterface)
{
	if (fileInterface_ == nullptr)
	{
		fileInterface_ = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	}
}

SoundLoader::~SoundLoader()
{
}

::Effekseer::SoundDataRef SoundLoader::Load(const char16_t* path)
{
	assert(path != nullptr);

	auto reader = fileInterface_->OpenRead(path);
	if (reader == nullptr)
		return nullptr;

	size_t size = reader->GetLength();
	std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
	size = reader->Read(data.get(), size);

	return Load(data.get(), (int32_t)size);
}

::Effekseer::SoundDataRef SoundLoader::Load(const void* data, int32_t size)
{
	osm::Sound* osmSound = sound_->GetDevice()->CreateSound(data, (int32_t)size, false);
	if (osmSound == nullptr)
	{
		return nullptr;
	}

	auto soundData = ::Effekseer::MakeRefPtr<SoundData>();
	soundData->osmSound_ = osmSound;
	return soundData;
}

void SoundLoader::Unload(::Effekseer::SoundDataRef soundData)
{
	if (soundData != nullptr)
	{
		sound_->StopData(soundData);
		SoundData* soundDataImpl = (SoundData*)soundData.Get();
		ES_SAFE_RELEASE(soundDataImpl->osmSound_);
	}
}

} // namespace EffekseerSound

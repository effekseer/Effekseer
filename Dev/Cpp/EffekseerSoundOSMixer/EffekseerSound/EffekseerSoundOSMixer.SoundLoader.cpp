
#include "EffekseerSoundOSMixer.SoundLoader.h"
#include "EffekseerSoundOSMixer.SoundImplemented.h"
#include <assert.h>
#include <memory>
#include <string.h>

namespace EffekseerSound
{

SoundLoader::SoundLoader(const SoundImplementedRef& sound, ::Effekseer::FileInterfaceRef fileInterface)
	: m_sound(sound)
	, m_fileInterface(fileInterface)
{
	if (m_fileInterface == nullptr)
	{
		m_fileInterface = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	}
}

SoundLoader::~SoundLoader()
{
}

::Effekseer::SoundDataRef SoundLoader::Load(const char16_t* path)
{
	assert(path != nullptr);

	auto reader = m_fileInterface->OpenRead(path);
	if (reader == nullptr)
		return nullptr;

	size_t size = reader->GetLength();
	std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
	size = reader->Read(data.get(), size);

	return Load(data.get(), (int32_t)size);
}

::Effekseer::SoundDataRef SoundLoader::Load(const void* data, int32_t size)
{
	osm::Sound* osmSound = m_sound->GetDevice()->CreateSound(data, (int32_t)size, false);
	if (osmSound == nullptr)
	{
		return nullptr;
	}

	auto soundData = ::Effekseer::MakeRefPtr<SoundData>();
	soundData->osmSound = osmSound;
	return soundData;
}

void SoundLoader::Unload(::Effekseer::SoundDataRef soundData)
{
	if (soundData != nullptr)
	{
		m_sound->StopData(soundData);
		SoundData* soundDataImpl = (SoundData*)soundData.Get();
		ES_SAFE_RELEASE(soundDataImpl->osmSound);
	}
}

} // namespace EffekseerSound

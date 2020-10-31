
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerSoundOSMixer.SoundLoader.h"
#include "EffekseerSoundOSMixer.SoundImplemented.h"
#include <assert.h>
#include <memory>
#include <string.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundLoader::SoundLoader(SoundImplemented* sound, ::Effekseer::FileInterface* fileInterface)
	: m_sound(sound)
	, m_fileInterface(fileInterface)
{
	if (m_fileInterface == nullptr)
	{
		m_fileInterface = &m_defaultFileInterface;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundLoader::~SoundLoader()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* SoundLoader::Load(const char16_t* path)
{
	assert(path != nullptr);

	std::unique_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));
	if (reader.get() == nullptr)
		return nullptr;

	size_t size = reader->GetLength();
	std::unique_ptr<uint8_t[]> data(new uint8_t[size]);
	size = reader->Read(data.get(), size);

	return Load(data.get(), (int32_t)size);
}

void* SoundLoader::Load(const void* data, int32_t size)
{
	return m_sound->GetDevice()->CreateSound(data, (int32_t)size, false);
}

void SoundLoader::Unload(void* data)
{
	SoundData* soundData = (SoundData*)data;
	if (soundData == nullptr)
	{
		return;
	}
	m_sound->StopData(soundData);
	soundData->Release();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

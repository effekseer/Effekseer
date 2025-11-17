
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerSoundXAudio2.SoundLoader.h"
#include "EffekseerSoundXAudio2.SoundImplemented.h"
#include <assert.h>
#include <memory>
#include <stdint.h>
#include <string.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{

namespace SupportXAudio2
{
class BinaryFileReader : public Effekseer::FileReader
{
private:
	uint8_t* origin_ = nullptr;
	int32_t pos_ = 0;
	int32_t size_ = 0;

public:
	BinaryFileReader(const void* data, int32_t size)
	{
		origin_ = (uint8_t*)data;
		size_ = size;
	}

	virtual ~BinaryFileReader()
	{
	}

	size_t Read(void* buffer, size_t size) override
	{
		if (pos_ + size > size_)
		{
			size = size_ - pos_;
		}

		memcpy(buffer, (origin_ + pos_), size);
		pos_ += static_cast<int32_t>(size);
		return size;
	}

	void Seek(int position) override
	{
		pos_ = position;
	}

	int GetPosition() const override
	{
		return pos_;
	}

	size_t GetLength() const override
	{
		return size_;
	}
};
} // namespace SupportXAudio2

SoundLoader::SoundLoader(const SoundImplementedRef& sound, ::Effekseer::FileInterfaceRef fileInterface)
	: sound_(sound)
	, fileInterface_(fileInterface)
{
	if (fileInterface_ == nullptr)
	{
		fileInterface_ = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundLoader::~SoundLoader()
{
}

::Effekseer::SoundDataRef SoundLoader::Load(::Effekseer::FileReaderRef reader)
{
	uint32_t chunkIdent, chunkSize;
	// check RIFF chunk
	reader->Read(&chunkIdent, 4);
	reader->Read(&chunkSize, 4);
	if (memcmp(&chunkIdent, "RIFF", 4) != 0)
	{
		return nullptr;
	}

	// check WAVE symbol
	reader->Read(&chunkIdent, 4);
	if (memcmp(&chunkIdent, "WAVE", 4) != 0)
	{
		return nullptr;
	}

	WAVEFORMATEX wavefmt = {0};
	for (;;)
	{
		reader->Read(&chunkIdent, 4);
		reader->Read(&chunkSize, 4);

		if (memcmp(&chunkIdent, "fmt ", 4) == 0)
		{
			// format chunk
			uint32_t size = (chunkSize < (uint32_t)sizeof(wavefmt)) ? chunkSize : (uint32_t)sizeof(wavefmt);
			reader->Read(&wavefmt, size);
			if (size < chunkSize)
			{
				reader->Seek(reader->GetPosition() + chunkSize - size);
			}
		}
		else if (memcmp(&chunkIdent, "data", 4) == 0)
		{
			// data chunk
			break;
		}
		else
		{
			// unknown chunk
			reader->Seek(reader->GetPosition() + chunkSize);
		}
	}

	// check a format
	if (wavefmt.wFormatTag != WAVE_FORMAT_PCM || wavefmt.nChannels > 2)
	{
		return nullptr;
	}

	uint8_t* buffer;
	uint32_t size;
	switch (wavefmt.wBitsPerSample)
	{
	case 8:
		// convert 8bit -> 16bit PCM
		size = chunkSize * 2;
		buffer = new uint8_t[size];
		reader->Read(&buffer[size / 2], chunkSize);
		{
			int16_t* dst = (int16_t*)&buffer[0];
			uint8_t* src = (uint8_t*)&buffer[size / 2];
			for (uint32_t i = 0; i < size; i += 2)
			{
				*dst++ = (int16_t)(((int32_t)*src++ - 128) << 8);
			}
		}
		break;
	case 16:
		// not convert
		buffer = new uint8_t[chunkSize];
		size = static_cast<uint32_t>(reader->Read(buffer, chunkSize));
		break;
	case 24:
		// convert 24bit -> 16bit PCM
		size = chunkSize * 2 / 3;
		buffer = new uint8_t[size];
		{
			uint8_t* chunkData = new uint8_t[chunkSize];
			reader->Read(chunkData, chunkSize);

			int16_t* dst = (int16_t*)&buffer[0];
			uint8_t* src = (uint8_t*)&chunkData[0];
			for (uint32_t i = 0; i < size; i += 2)
			{
				*dst++ = (int16_t)(src[1] | (src[2] << 8));
				src += 3;
			}
			delete[] chunkData;
		}
		break;
	default:
		return nullptr;
	}

	SoundDataRef soundData = ::Effekseer::MakeRefPtr<SoundData>();
	soundData->channels_ = wavefmt.nChannels;
	soundData->sampleRate_ = wavefmt.nSamplesPerSec;
	soundData->buffer_.Flags = XAUDIO2_END_OF_STREAM;
	soundData->buffer_.AudioBytes = size;
	soundData->buffer_.pAudioData = (BYTE*)buffer;

	return soundData;
}

::Effekseer::SoundDataRef SoundLoader::Load(const char16_t* path)
{
	assert(path != nullptr);

	auto reader = fileInterface_->OpenRead(path);
	if (reader == nullptr)
		return nullptr;

	return Load(reader);
}

::Effekseer::SoundDataRef SoundLoader::Load(const void* data, int32_t size)
{
	auto reader = Effekseer::MakeRefPtr<SupportXAudio2::BinaryFileReader>(data, size);
	return Load(reader);
}

void SoundLoader::Unload(::Effekseer::SoundDataRef soundData)
{
	if (soundData != nullptr)
	{
		// stop a voice which plays this data
		sound_->StopData(soundData);
		SoundData* soundDataImpl = (SoundData*)soundData.Get();
		ES_SAFE_DELETE_ARRAY(soundDataImpl->buffer_.pAudioData);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

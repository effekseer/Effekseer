
#include "EffekseerSoundAL.SoundLoader.h"
#include "EffekseerSoundAL.SoundImplemented.h"
#include <assert.h>
#include <memory>
#include <string.h>

namespace EffekseerSound
{

namespace SupportOpenAL
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
		pos_ += size;
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
} // namespace SupportOpenAL

SoundLoader::SoundLoader(::Effekseer::FileInterfaceRef fileInterface)
	: fileInterface_(fileInterface)
{
	if (fileInterface_ == nullptr)
	{
		fileInterface_ = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	}
}

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
		return NULL;
	}

	// check WAVE Symbol
	reader->Read(&chunkIdent, 4);
	if (memcmp(&chunkIdent, "WAVE", 4) != 0)
	{
		return NULL;
	}

	struct
	{
		uint16_t wFormatTag;
		uint16_t nChannels;
		uint32_t nSamplesPerSec;
		uint32_t nAvgBytesPerSec;
		uint16_t nBlockAlign;
		uint16_t wBitsPerSample;
		uint16_t cbSize;
	} wavefmt = {0};

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

	// check format
	if (wavefmt.wFormatTag != 1 || wavefmt.nChannels > 2)
	{
		return NULL;
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
		size = reader->Read(buffer, chunkSize);
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
		return NULL;
	}

	ALenum format = (wavefmt.nChannels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

	auto soundData = ::Effekseer::MakeRefPtr<SoundData>();
	soundData->channels_ = wavefmt.nChannels;
	soundData->sampleRate_ = wavefmt.nSamplesPerSec;
	alGenBuffers(1, &soundData->buffer_);
	alBufferData(soundData->buffer_, format, buffer, size, wavefmt.nSamplesPerSec);
	delete[] buffer;

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
	auto reader = Effekseer::MakeRefPtr<SupportOpenAL::BinaryFileReader>(data, size);
	return Load(reader);
}

void SoundLoader::Unload(::Effekseer::SoundDataRef soundData)
{
	if (soundData != nullptr)
	{
		SoundData* soundDataImpl = (SoundData*)soundData.Get();
		alDeleteBuffers(1, &soundDataImpl->buffer_);
		soundData = nullptr;
	}
}

} // namespace EffekseerSound

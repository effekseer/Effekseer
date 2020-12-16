
#include <assert.h>
#include <string.h>
#include <memory>
#include "EffekseerSoundAL.SoundImplemented.h"
#include "EffekseerSoundAL.SoundLoader.h"

namespace EffekseerSound
{

namespace SupportOpenAL
{
class BinaryFileReader : public Effekseer::FileReader
{
private:
	uint8_t* origin = nullptr;
	int32_t pos = 0;
	int32_t size_ = 0;

public:
	BinaryFileReader(const void* data, int32_t size)
	{
		origin = (uint8_t*)data;
		size_ = size;
	}

	virtual ~BinaryFileReader() {}

	size_t Read(void* buffer, size_t size) override
	{
		if (pos + size > size_)
		{
			size = size_ - pos;
		}

		memcpy(buffer, (origin + pos), size);
		pos += size;
		return size;
	}

	void Seek(int position) override { pos = position; }

	int GetPosition() override { return pos; }

	size_t GetLength() override { return size_; }
};
}

SoundLoader::SoundLoader( ::Effekseer::FileInterface* fileInterface )
	: m_fileInterface( fileInterface )
{
	if( m_fileInterface == NULL )
	{ 
		m_fileInterface = &m_defaultFileInterface;
	}
}

SoundLoader::~SoundLoader()
{
}

::Effekseer::SoundDataRef SoundLoader::Load(::Effekseer::FileReader* reader) {
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
	soundData->channels = wavefmt.nChannels;
	soundData->sampleRate = wavefmt.nSamplesPerSec;
	alGenBuffers(1, &soundData->buffer);
	alBufferData(soundData->buffer, format, buffer, size, wavefmt.nSamplesPerSec);
	delete[] buffer;

	return soundData;
}

::Effekseer::SoundDataRef SoundLoader::Load( const char16_t* path )
{
	assert( path != NULL );
	
	std::unique_ptr<::Effekseer::FileReader> 
		reader( m_fileInterface->OpenRead( path ) );
	if( reader.get() == NULL ) return NULL;

	return Load(reader.get());
}
	
::Effekseer::SoundDataRef SoundLoader::Load(const void* data, int32_t size)
{
	auto reader = SupportOpenAL::BinaryFileReader(data, size);
	return Load(&reader);
}

void SoundLoader::Unload( ::Effekseer::SoundDataRef soundData )
{
	if (soundData != nullptr)
	{
		SoundData* soundDataImpl = (SoundData*)soundData.Get();
		alDeleteBuffers(1, &soundDataImpl->buffer);
		soundData = nullptr;
	}
}

}

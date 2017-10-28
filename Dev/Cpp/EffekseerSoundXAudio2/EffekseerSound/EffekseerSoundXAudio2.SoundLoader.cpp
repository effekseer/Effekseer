
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <assert.h>
#include <string.h>
#include <memory>
#include "EffekseerSoundXAudio2.SoundImplemented.h"
#include "EffekseerSoundXAudio2.SoundLoader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundLoader::SoundLoader( SoundImplemented* sound, ::Effekseer::FileInterface* fileInterface )
	: m_sound	( sound )
	, m_fileInterface( fileInterface )
{
	if( m_fileInterface == NULL )
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
void* SoundLoader::Load( const EFK_CHAR* path )
{
	assert( path != NULL );
	
	std::unique_ptr<::Effekseer::FileReader> 
		reader( m_fileInterface->OpenRead( path ) );
	if( reader.get() == NULL ) return false;

	uint32_t chunkIdent, chunkSize;
	// RIFFチャンクをチェック
	reader->Read(&chunkIdent, 4);
	reader->Read(&chunkSize, 4);
	if (memcmp(&chunkIdent, "RIFF", 4) != 0) {
		return NULL;
	}

	// WAVEシンボルをチェック
	reader->Read(&chunkIdent, 4);
	if (memcmp(&chunkIdent, "WAVE", 4) != 0) {
		return NULL;
	}
	
	WAVEFORMATEX wavefmt = {0};
	for (;;) {
		reader->Read(&chunkIdent, 4);
		reader->Read(&chunkSize, 4);

		if (memcmp(&chunkIdent, "fmt ", 4) == 0) {
			// フォーマットチャンク
			uint32_t size = (chunkSize < (uint32_t)sizeof(wavefmt)) ? chunkSize : (uint32_t)sizeof(wavefmt);
			reader->Read(&wavefmt, size);
			if (size < chunkSize) {
				reader->Seek(reader->GetPosition() + chunkSize - size);
			}
		} else if (memcmp(&chunkIdent, "data", 4) == 0) {
			// データチャンク
			break;
		} else {
			// 不明なチャンクはスキップ
			reader->Seek(reader->GetPosition() + chunkSize);
		}
	}
	
	// フォーマットチェック
	if (wavefmt.wFormatTag != WAVE_FORMAT_PCM || wavefmt.nChannels > 2) {
		return NULL;
	}

	uint8_t* buffer;
	uint32_t size;
	switch (wavefmt.wBitsPerSample) {
	case 8:
		// 8bit -> 16bit PCM変換
		size = chunkSize * 2;
		buffer = new uint8_t[size];
		reader->Read(&buffer[size / 2], chunkSize);
		{
			int16_t* dst = (int16_t*)&buffer[0];
			uint8_t* src = (uint8_t*)&buffer[size / 2];
			for (uint32_t i = 0; i < size; i += 2) {
				*dst++ = (int16_t)(((int32_t)*src++ - 128) << 8);
			}
		}
		break;
	case 16:
		// そのまま読み込み
		buffer = new uint8_t[chunkSize];
		size = reader->Read(buffer, chunkSize);
		break;
	case 24:
		// 24bit -> 16bit PCM変換
		size = chunkSize * 2 / 3;
		buffer = new uint8_t[size];
		{
			uint8_t* chunkData = new uint8_t[chunkSize];
			reader->Read(chunkData, chunkSize);

			int16_t* dst = (int16_t*)&buffer[0];
			uint8_t* src = (uint8_t*)&chunkData[0];
			for (uint32_t i = 0; i < size; i += 2) {
				*dst++ = (int16_t)(src[1] | (src[2] << 8));
				src += 3;
			}
			delete[] chunkData;
		}
		break;
	default:
		return NULL;
	}

	SoundData* soundData = new SoundData;
	memset(soundData, 0, sizeof(SoundData));
	soundData->channels = wavefmt.nChannels;
	soundData->sampleRate = wavefmt.nSamplesPerSec;
	soundData->buffer.Flags = XAUDIO2_END_OF_STREAM;
	soundData->buffer.AudioBytes = size;
	soundData->buffer.pAudioData = (BYTE*)buffer;

	return soundData;
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundLoader::Unload( void* data )
{
	SoundData* soundData = (SoundData*)data;
	if (soundData == NULL) {
		return;
	}
	// このデータを再生しているボイスを停止させる
	m_sound->StopData( soundData );

	delete[] soundData->buffer.pAudioData;
	delete soundData;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

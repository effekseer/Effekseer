
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
			uint32_t size = min(chunkSize, sizeof(wavefmt));
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

	BYTE* buffer;
	uint32_t size;
	switch (wavefmt.wBitsPerSample) {
	case 8:
		// 16bitPCMに変換
		size = chunkSize * 2;
		buffer = new BYTE[size];
		reader->Read(&buffer[size / 2], chunkSize);
		{
			int16_t* dst = (int16_t*)&buffer[0];
			uint8_t* src = (uint8_t*)&buffer[size / 2];
			for (uint32_t i = 0; i < chunkSize; i++) {
				*dst++ = (int16_t)(((int32_t)*src++ - 128) << 8);
			}
		}
		break;
	case 16:
		// そのまま読み込み
		buffer = new BYTE[chunkSize];
		size = reader->Read(buffer, chunkSize);
		break;
	}

	SoundData* soundData = new SoundData;
	memset(soundData, 0, sizeof(SoundData));
	soundData->channels = wavefmt.nChannels;
	soundData->sampleRate = wavefmt.nSamplesPerSec;
	soundData->buffer.Flags = XAUDIO2_END_OF_STREAM;
	soundData->buffer.AudioBytes = size;
	soundData->buffer.pAudioData = buffer;
	
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

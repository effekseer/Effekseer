
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <string.h>
#include "EffekseerSoundDSound.SoundImplemented.h"
#include "EffekseerSoundDSound.SoundLoader.h"
#include <algorithm>
#include <memory>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundLoader::SoundLoader( SoundImplemented* sound, ::Effekseer::FileInterface* fileInterface )
	: m_sound(sound), m_fileInterface(fileInterface)
{
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

	HRESULT hr;
	
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

	// DirectSoundバッファを作成
	DSBUFFERDESC dsdesc;
    ZeroMemory(&dsdesc,sizeof(DSBUFFERDESC));
    dsdesc.dwSize = sizeof(DSBUFFERDESC);
	dsdesc.dwFlags = DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLVOLUME | 
		DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN;
    dsdesc.dwBufferBytes = size;
    dsdesc.lpwfxFormat = &wavefmt;
    dsdesc.guid3DAlgorithm = DS3DALG_DEFAULT;

	IDirectSoundBuffer* dsbufTmp = 0;
	IDirectSoundBuffer8* dsbuf = NULL;
	hr = m_sound->GetDevice()->CreateSoundBuffer(&dsdesc, &dsbufTmp, NULL);
	if (hr == DS_OK) {
		hr = dsbufTmp->QueryInterface(IID_IDirectSoundBuffer8, (void**)&dsbuf);
		dsbufTmp->Release();
	}
	if (hr != DS_OK) {
		delete[] buffer;
        return NULL;
    }

    // バッファをロックしてロード
    LPVOID bufptr;
    DWORD bufsize;
	hr = dsbuf->Lock(0, 0, &bufptr, &bufsize, NULL, NULL, DSBLOCK_ENTIREBUFFER);
	if (hr == DS_OK) {
		memcpy(bufptr, buffer, size);
		hr = dsbuf->Unlock(bufptr, bufsize, NULL, 0);
	}
	delete[] buffer;

	SoundData* soundData = new SoundData;
	soundData->channels = wavefmt.nChannels;
	soundData->sampleRate = wavefmt.nSamplesPerSec;
	soundData->buffer = dsbuf;

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

	m_sound->StopData(soundData);
	soundData->buffer->Release();
	delete soundData;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

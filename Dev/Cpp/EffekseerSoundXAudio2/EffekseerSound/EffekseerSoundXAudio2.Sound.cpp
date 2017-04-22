
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../EffekseerSoundXAudio2.h"
#include "EffekseerSoundXAudio2.SoundPlayer.h"
#include "EffekseerSoundXAudio2.SoundVoice.h"
#include "EffekseerSoundXAudio2.SoundLoader.h"
#include "EffekseerSoundXAudio2.SoundImplemented.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerSound
{

static float StereoLayout[] = {
	3 * X3DAUDIO_PI / 2,
	X3DAUDIO_PI / 2
};
const int32_t DefaultSampleRate = 44100;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Sound* Sound::Create( IXAudio2* xaudio2, int32_t num1chVoices, int32_t num2chVoices )
{
	SoundImplemented* sound = new SoundImplemented();
	if( sound->Initialize( xaudio2, num1chVoices, num2chVoices ) )
	{
		return sound;
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundImplemented::SoundImplemented()
	: m_xaudio2	( NULL )
	, m_mute	( false )
{
	memset(m_voiceContainer, 0, sizeof(m_voiceContainer));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundImplemented::~SoundImplemented()
{
	StopAllVoices();
	for (int i = 0; i < 2; i++) {
		delete m_voiceContainer[i];
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundImplemented::Initialize( IXAudio2* xaudio2, int32_t num1chVoices, int32_t num2chVoices )
{
	m_xaudio2 = xaudio2;
	
	// X3DAudioを初期化
	X3DAudioInitialize(SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT, 
		X3DAUDIO_SPEED_OF_SOUND, m_x3daudio);

	// 入力ボイスを作成
	WAVEFORMATEX format = {0};
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.wBitsPerSample = 16;
	format.nSamplesPerSec = DefaultSampleRate;
	
	// モノラルボイスを作成
	format.nChannels = 1;
	format.nBlockAlign = format.wBitsPerSample / 8 * format.nChannels;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
	m_voiceContainer[0] = new SoundVoiceContainer(this, num1chVoices, &format);
	
	// ステレオボイスを作成
	format.nChannels = 2;
	format.nBlockAlign = format.wBitsPerSample / 8 * format.nChannels;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
	m_voiceContainer[1] = new SoundVoiceContainer(this, num2chVoices, &format);

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::SetListener( const ::Effekseer::Vector3D& pos, 
		const ::Effekseer::Vector3D& at, const ::Effekseer::Vector3D& up )
{
	::Effekseer::Vector3D front;
	::Effekseer::Vector3D::Sub(front, at, pos);

	memset(&m_listener, 0, sizeof(m_listener));
	m_listener.OrientTop = *(X3DAUDIO_VECTOR*)&up;
	m_listener.OrientFront.x = -front.X;
	m_listener.OrientFront.y = -front.Y;
	m_listener.OrientFront.z = -front.Z;
	m_listener.Position = *(X3DAUDIO_VECTOR*)&pos;
	m_listener.Velocity.x = 0.0f;
	m_listener.Velocity.y = 0.0f;
	m_listener.Velocity.z = 0.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::Destroy()
{
	delete this;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SoundPlayer* SoundImplemented::CreateSoundPlayer()
{
	return new SoundPlayer( this );
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SoundLoader* SoundImplemented::CreateSoundLoader( ::Effekseer::FileInterface* fileInterface )
{
	return new SoundLoader( this, fileInterface );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::StopAllVoices()
{
	for (int i = 0; i < 2; i++) {
		m_voiceContainer[i]->StopAll();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::SetMute( bool mute )
{
	m_mute = mute;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice* SoundImplemented::GetVoice( int32_t channel )
{
	IXAudio2SourceVoice* voice = NULL;
	if (channel >= 1 && channel <= 2) {
		return m_voiceContainer[channel - 1]->GetVoice();
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::StopTag( ::Effekseer::SoundTag tag )
{
	for (int i = 0; i < 2; i++) {
		m_voiceContainer[i]->StopTag(tag);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::PauseTag( ::Effekseer::SoundTag tag, bool pause )
{
	for (int i = 0; i < 2; i++) {
		m_voiceContainer[i]->PauseTag(tag, pause);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundImplemented::CheckPlayingTag( ::Effekseer::SoundTag tag )
{
	for (int i = 0; i < 2; i++) {
		if (m_voiceContainer[i]->CheckPlayingTag(tag)) {
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::StopData( SoundData* soundData )
{
	for (int i = 0; i < 2; i++) {
		m_voiceContainer[i]->StopData(soundData);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::Calculate3DSound(const ::Effekseer::Vector3D& position, 
	float distance, int32_t input, int32_t output, float matrix[])
{
	const X3DAUDIO_VECTOR vecTop = { 0.0f, 1.0f, 0.0f };
	const X3DAUDIO_VECTOR vecFront = { 0.0f, 0.0f, 1.0f };

	X3DAUDIO_DSP_SETTINGS settings = {0};
	X3DAUDIO_EMITTER emitter = {0};

	emitter.ChannelCount = input;
	emitter.pChannelAzimuths = StereoLayout;
	emitter.Position = *(X3DAUDIO_VECTOR*)&position;
	emitter.InnerRadius = distance;
	emitter.CurveDistanceScaler = distance;
	emitter.OrientFront = vecFront;
	emitter.OrientTop = vecTop;
	
	settings.SrcChannelCount = input;
	settings.DstChannelCount = output;
	settings.pMatrixCoefficients = matrix;
	X3DAudioCalculate(m_x3daudio, &m_listener, &emitter, 
		X3DAUDIO_CALCULATE_MATRIX, &settings);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

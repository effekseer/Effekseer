
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../EffekseerSoundXAudio2.h"
#include "EffekseerSoundXAudio2.SoundImplemented.h"
#include "EffekseerSoundXAudio2.SoundLoader.h"
#include "EffekseerSoundXAudio2.SoundPlayer.h"
#include "EffekseerSoundXAudio2.SoundVoice.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerSound
{

static float StereoLayout[] = {3 * X3DAUDIO_PI / 2, X3DAUDIO_PI / 2};
const int32_t DefaultSampleRate = 44100;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundRef Sound::Create(IXAudio2* xaudio2, int32_t num1chVoices, int32_t num2chVoices)
{
	auto sound = Effekseer::MakeRefPtr<SoundImplemented>();
	if (sound->Initialize(xaudio2, num1chVoices, num2chVoices))
	{
		return sound;
	}
	return nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundImplemented::SoundImplemented() = default;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundImplemented::~SoundImplemented()
{
	StopAllVoices();
	for (int i = 0; i < 2; i++)
	{
		delete voiceContainer_[i];
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundImplemented::Initialize(IXAudio2* xaudio2, int32_t num1chVoices, int32_t num2chVoices)
{
	xaudio2_ = xaudio2;

	// X3DAudioを初期化
	X3DAudioInitialize(SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT, X3DAUDIO_SPEED_OF_SOUND, x3daudio_);

	// 入力ボイスを作成
	WAVEFORMATEX format = {0};
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.wBitsPerSample = 16;
	format.nSamplesPerSec = DefaultSampleRate;

	// モノラルボイスを作成
	format.nChannels = 1;
	format.nBlockAlign = format.wBitsPerSample / 8 * format.nChannels;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
	voiceContainer_[0] = new SoundVoiceContainer(this, num1chVoices, &format);

	// ステレオボイスを作成
	format.nChannels = 2;
	format.nBlockAlign = format.wBitsPerSample / 8 * format.nChannels;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
	voiceContainer_[1] = new SoundVoiceContainer(this, num2chVoices, &format);

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::SetListener(const ::Effekseer::Vector3D& pos, const ::Effekseer::Vector3D& at, const ::Effekseer::Vector3D& up)
{
	::Effekseer::Vector3D front;
	::Effekseer::Vector3D::Sub(front, at, pos);

	memset(&listener_, 0, sizeof(listener_));
	listener_.OrientTop = *(X3DAUDIO_VECTOR*)&up;
	listener_.OrientFront.x = -front.X;
	listener_.OrientFront.y = -front.Y;
	listener_.OrientFront.z = -front.Z;
	listener_.Position = *(X3DAUDIO_VECTOR*)&pos;
	listener_.Velocity.x = 0.0f;
	listener_.Velocity.y = 0.0f;
	listener_.Velocity.z = 0.0f;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::Destroy()
{
	Release();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SoundPlayerRef SoundImplemented::CreateSoundPlayer()
{
	return ::Effekseer::MakeRefPtr<SoundPlayer>(SoundImplementedRef::FromPinned(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SoundLoaderRef SoundImplemented::CreateSoundLoader(::Effekseer::FileInterfaceRef fileInterface)
{
	return ::Effekseer::MakeRefPtr<SoundLoader>(SoundImplementedRef::FromPinned(this), fileInterface);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::StopAllVoices()
{
	for (int i = 0; i < 2; i++)
	{
		voiceContainer_[i]->StopAll();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::SetMute(bool mute)
{
	mute_ = mute;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundVoice* SoundImplemented::GetVoice(int32_t channel)
{
	IXAudio2SourceVoice* voice = nullptr;
	if (channel >= 1 && channel <= 2)
	{
		return voiceContainer_[channel - 1]->GetVoice();
	}
	return nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::StopTag(::Effekseer::SoundTag tag)
{
	for (int i = 0; i < 2; i++)
	{
		voiceContainer_[i]->StopTag(tag);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::PauseTag(::Effekseer::SoundTag tag, bool pause)
{
	for (int i = 0; i < 2; i++)
	{
		voiceContainer_[i]->PauseTag(tag, pause);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool SoundImplemented::CheckPlayingTag(::Effekseer::SoundTag tag)
{
	for (int i = 0; i < 2; i++)
	{
		if (voiceContainer_[i]->CheckPlayingTag(tag))
		{
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::StopData(const ::Effekseer::SoundDataRef& soundData)
{
	for (int i = 0; i < 2; i++)
	{
		voiceContainer_[i]->StopData(soundData);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundImplemented::Calculate3DSound(
	const ::Effekseer::Vector3D& position, float distance, int32_t input, int32_t output, float matrix[])
{
	const X3DAUDIO_VECTOR vecTop = {0.0f, 1.0f, 0.0f};
	const X3DAUDIO_VECTOR vecFront = {0.0f, 0.0f, 1.0f};

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
	X3DAudioCalculate(x3daudio_, &listener_, &emitter, X3DAUDIO_CALCULATE_MATRIX, &settings);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

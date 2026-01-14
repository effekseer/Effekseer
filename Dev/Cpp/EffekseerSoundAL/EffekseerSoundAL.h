
#ifndef __EFFEKSEERSOUND_BASE_PRE_H__
#define __EFFEKSEERSOUND_BASE_PRE_H__

#if defined(_WIN32)
#include <AL/al.h>
#elif defined(__APPLE__)
#include <OpenAl/al.h>
#else
#include <AL/al.h>
#endif

#include <Effekseer.h>

namespace EffekseerSound
{
class Sound;
}

#endif // __EFFEKSEERSOUND_BASE_PRE_H__

#ifndef __EFFEKSEERSOUND_AL_H__
#define __EFFEKSEERSOUND_AL_H__

namespace EffekseerSound
{

/**
	@brief	~english Sound data
	@brief	~japanese サウンドデータ
*/
class SoundData : public ::Effekseer::SoundData
{
public:
	int32_t GetChannels() const
	{
		return channels_;
	}
	int32_t GetSampleRate() const
	{
		return sampleRate_;
	}
	ALuint GetBuffer() const
	{
		return buffer_;
	}

private:
	friend class SoundLoader;

	int32_t channels_ = 0;

	int32_t sampleRate_ = 0;

	ALuint buffer_ = 0;
};
using SoundDataRef = Effekseer::RefPtr<SoundData>;

/**
	@brief	~english Sound class
	@brief	~japanese サウンドクラス
*/
class Sound;
using SoundRef = Effekseer::RefPtr<Sound>;

class Sound : public Effekseer::IReference
{
protected:
	Sound()
	{
	}
	virtual ~Sound()
	{
	}

public:
	/**
		@brief	~english Create an instance.
		@brief	~japanese インスタンスを生成する。
		@param	numVoices	[in]	~english Number of voices
		@param	numVoices	[in]	~japanese ボイス数
		@return	~english Instance
		@return	~japanese インスタンス
	*/
	static SoundRef Create(int32_t numVoices);

	/**
		@brief	~english Destroy this instance.
		@brief	~japanese このインスタンスを破棄する。
	*/
	virtual void Destroy() = 0;

	/**
		@brief	~english Set listener.
		@brief	~japanese リスナー設定。
	*/
	virtual void SetListener(const ::Effekseer::Vector3D& pos,
							 const ::Effekseer::Vector3D& at,
							 const ::Effekseer::Vector3D& up) = 0;

	/**
		@brief	~english Create a sound player.
		@brief	~japanese サウンドプレイヤを生成する。
	*/
	virtual ::Effekseer::SoundPlayerRef CreateSoundPlayer() = 0;

	/**
		@brief	~english Create a default sound loader.
		@brief	~japanese 標準のサウンド読込インスタンスを生成する。
	*/
	virtual ::Effekseer::SoundLoaderRef CreateSoundLoader(::Effekseer::FileInterfaceRef fileInterface = NULL) = 0;

	/**
		@brief	~english Stop all voices.
		@brief	~japanese 全発音を停止。
	*/
	virtual void StopAllVoices() = 0;

	/**
		@brief	~english Set mute.
		@brief	~japanese ミュート設定。
	*/
	virtual void SetMute(bool mute) = 0;
};

} // namespace EffekseerSound

#endif // __EFFEKSEERSOUND_AL_H__

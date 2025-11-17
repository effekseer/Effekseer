
#ifndef __EFFEKSEERSOUND_BASE_PRE_H__
#define __EFFEKSEERSOUND_BASE_PRE_H__

#include <Effekseer.h>

namespace EffekseerSound
{
class Sound;
} // namespace EffekseerSound

#endif // __EFFEKSEERSOUND_BASE_PRE_H__

#ifndef __EFFEKSEERSOUND_OSMIXER_H__
#define __EFFEKSEERSOUND_OSMIXER_H__

#include <OpenSoundMixer.h>

namespace EffekseerSound
{

class SoundData : public ::Effekseer::SoundData
{
public:
	const osm::Sound* GetOsmSound() const
	{
		return osmSound_;
	}

private:
	friend class SoundLoader;

	int32_t channels_ = 0;
	int32_t sampleRate_ = 0;
	osm::Sound* osmSound_ = nullptr;
};

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
		@param	device	[in]	~english OSMixer device
		@param	device	[in]	~japanese OSMixerのデバイス
		@param	voiceCount1ch	[in]	~english Number of mono voices
		@param	voiceCount1ch	[in]	~japanese モノラルボイス数
		@param	voiceCount2ch	[in]	~english Number of stereo voices
		@param	voiceCount2ch	[in]	~japanese ステレオボイス数
		@return	~english Instance
		@return	~japanese インスタンス
	*/
	static SoundRef Create(osm::Manager* soundManager);

	/**
		@brief	~english Destroy this instance.
		@brief	~japanese このインスタンスを破棄する。
	*/
	virtual void Destroy() = 0;

	/**
		@brief	~english Update
		@brief	~japanese 更新
	*/
	virtual void Update() = 0;

	/**
		@brief	~english Set listener
		@brief	~japanese リスナー設定
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
		@brief	~english Set mute.
		@brief	~japanese ミュート設定
	*/
	virtual void SetMute(bool mute) = 0;
};

} // namespace EffekseerSound

#endif // __EFFEKSEERSOUND_OSMIXER_H__

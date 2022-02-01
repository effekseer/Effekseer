
#ifndef __EFFEKSEERSOUND_BASE_PRE_H__
#define __EFFEKSEERSOUND_BASE_PRE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Sound;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERSOUND_BASE_PRE_H__

#ifndef __EFFEKSEERSOUND_OSMIXER_H__
#define __EFFEKSEERSOUND_OSMIXER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <OpenSoundMixer.h>

//----------------------------------------------------------------------------------
// Lib
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class SoundData : public ::Effekseer::SoundData
{
public:
	const osm::Sound* GetOsmSound() const
	{
		return osmSound;
	}

private:
	friend class SoundLoader;

	/**
		@brief	チャンネル数。
	*/
	int32_t channels;

	/**
		@brief	サンプリング周波数。
	*/
	int32_t sampleRate;

	/**
		@brief	OpenSoundMixerバッファ。
	*/
	osm::Sound* osmSound;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	サウンドクラス
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
		@brief	インスタンスを生成する。
		@param	device	[in]	OSMixerのデバイス
		@param	voiceCount1ch	[in]	モノラルボイス数
		@param	voiceCount2ch	[in]	ステレオボイス数
		@return	インスタンス
	*/
	static SoundRef Create(osm::Manager* soundManager);

	/**
		@brief	このインスタンスを破棄する。
	*/
	virtual void Destroy() = 0;

	/**
		@brief	更新
	*/
	virtual void Update() = 0;

	/**
		@brief	リスナー設定
	*/
	virtual void SetListener(const ::Effekseer::Vector3D& pos,
							 const ::Effekseer::Vector3D& at,
							 const ::Effekseer::Vector3D& up) = 0;

	/**
		@brief	サウンドプレイヤを生成する。
	*/
	virtual ::Effekseer::SoundPlayerRef CreateSoundPlayer() = 0;

	/**
		@brief	標準のサウンド読込インスタンスを生成する。
	*/
	virtual ::Effekseer::SoundLoaderRef CreateSoundLoader(::Effekseer::FileInterfaceRef fileInterface = NULL) = 0;

	/**
		@brief	ミュート設定
	*/
	virtual void SetMute(bool mute) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerSound
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERSOUND_OSMIXER_H__

#ifndef __EFFEKSEERTOOL_SOUND_H__
#define __EFFEKSEERTOOL_SOUND_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerSoundOSMixer.h>
#include <OpenSoundMixer.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerTool
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Sound
{
private:
	::EffekseerSound::Sound* m_sound;
	osm::Manager* m_manager;
	float m_volume;
	bool m_mute;

public:
	/**
		@brief	コンストラクタ
	*/
	Sound();

	/**
		@brief	デストラクタ
	*/
	~Sound();

	/**
		@brief	初期化を行う。
	*/
	bool Initialize();

	/**
		@brief	更新を行う。
	*/
	void Update();

	/**
		@brief	ボリューム設定。
	*/
	void SetVolume(float volume);

	/**
		@brief	無音設定。
	*/
	void SetMute(bool mute);

	float GetVolume()
	{
		return m_volume;
	}
	bool GetMute()
	{
		return m_mute;
	}

	/**
		@brief	リスナー位置設定。
	*/
	void SetListener(const Effekseer::Vector3D& pos, const Effekseer::Vector3D& at, const Effekseer::Vector3D& up);

	::EffekseerSound::Sound* GetSound()
	{
		return m_sound;
	};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerTool
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERTOOL_SOUND_H__
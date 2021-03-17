
#ifndef	__EFFEKSEERSOUND_BASE_PRE_H__
#define	__EFFEKSEERSOUND_BASE_PRE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#if defined(_WIN32)
#include <AL/al.h>
#elif defined(__APPLE__)
#include <OpenAl/al.h>
#else
#include <AL/al.h>
#endif

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
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERSOUND_BASE_PRE_H__

#ifndef	__EFFEKSEERSOUND_AL_H__
#define	__EFFEKSEERSOUND_AL_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

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
	
/**
	@brief	サウンドデータ
*/
class SoundData : public ::Effekseer::SoundData
{	
public:
	int32_t GetChannels() const  { return channels; }
	int32_t GetSampleRate() const  { return sampleRate; }
	ALuint GetBuffer() const { return buffer; }

private:
	friend class SoundLoader;

	/**
		@brief	チャンネル数。
	*/
	int32_t			channels;
	
	/**
		@brief	サンプリング周波数。
	*/
	int32_t			sampleRate;

	/**
		@brief	OpenALバッファ。
	*/
	ALuint			buffer;
};
using SoundDataRef = Effekseer::RefPtr<SoundData>;

/**
	@brief	サウンドクラス
*/
class Sound;
using SoundRef = Effekseer::RefPtr<Sound>;

class Sound : public Effekseer::IReference
{
protected:
	Sound() {}
	virtual ~Sound() {}

public:
	/**
		@brief	インスタンスを生成する。
		@param	device	[in]	XAudio2のデバイス
		@param	voiceCount1ch	[in]	モノラルボイス数
		@param	voiceCount2ch	[in]	ステレオボイス数
		@return	インスタンス
	*/
	static SoundRef Create( int32_t numVoices );
	
	/**
		@brief	このインスタンスを破棄する。
	*/
	virtual void Destroy() = 0;
	
	/**
		@brief	リスナー設定
	*/
	virtual void SetListener( const ::Effekseer::Vector3D& pos, 
		const ::Effekseer::Vector3D& at, const ::Effekseer::Vector3D& up ) = 0;
	
	/**
		@brief	サウンドプレイヤを生成する。
	*/
	virtual ::Effekseer::SoundPlayerRef CreateSoundPlayer() = 0;

	/**
		@brief	標準のサウンド読込インスタンスを生成する。
	*/
	virtual ::Effekseer::SoundLoaderRef CreateSoundLoader( ::Effekseer::FileInterface* fileInterface = NULL ) = 0;
	
	/**
		@brief	全発音を停止
	*/
	virtual void StopAllVoices() = 0;

	/**
		@brief	ミュート設定
	*/
	virtual void SetMute( bool mute ) = 0;

};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERSOUND_AL_H__
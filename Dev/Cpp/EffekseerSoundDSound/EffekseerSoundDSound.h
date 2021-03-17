
#ifndef	__EFFEKSEERSOUND_BASE_PRE_H__
#define	__EFFEKSEERSOUND_BASE_PRE_H__

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
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERSOUND_BASE_PRE_H__

#ifndef	__EFFEKSEERSOUND_DSOUND_H__
#define	__EFFEKSEERSOUND_DSOUND_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <dsound.h>

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
	int32_t GetChannels() const { return channels; }
	int32_t GetSampleRate() const { return sampleRate; }
	const IDirectSoundBuffer8* GetBuffer() const { return buffer; }

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
		@brief	DirectSoundバッファ。
	*/
	IDirectSoundBuffer8* buffer;
};
using SoundDataRef = ::Effekseer::RefPtr<SoundData>;

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
		@param	dsound	[in]	DirectSound8のデバイス
		@return	インスタンス
	*/
	static SoundRef Create( IDirectSound8* dsound );
	
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
	virtual ::Effekseer::SoundLoaderRef CreateSoundLoader(::Effekseer::FileInterface* fileInterface) = 0;
	
	/**
		@brief	全発音を停止
	*/
	virtual void StopAllVoices() = 0;

	/**
		@brief	ミュート設定
	*/
	virtual void SetMute( bool mute ) = 0;

	/**
		@brief	2Dパン用の範囲設定
	*/
	virtual void SetPanRange( int32_t leftPos, int32_t rightPos ) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERSOUND_DSOUND_H__
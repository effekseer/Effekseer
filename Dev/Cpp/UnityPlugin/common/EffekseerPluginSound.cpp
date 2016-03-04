#include <algorithm>
#include "EffekseerPluginSound.h"

namespace EffekseerPlugin
{
	void* SoundLoader::Load( const EFK_CHAR* path ){
		// リソーステーブルを検索して存在したらそれを使う
		auto it = resources.find((const char16_t*)path);
		if (it != resources.end()) {
			it->second.referenceCount++;
			return (void*)it->second.soundID;
		}

		// Unityでロード
		SoundResource res;
		res.soundID = load( (const char16_t*)path );
		if (res.soundID == 0) {
			return 0;
		}
		
		// リソーステーブルに追加
		resources.insert( std::make_pair((const char16_t*)path, res ) );
		
		return (void*)res.soundID;
	}

	void SoundLoader::Unload( void* source ){
		if (source == nullptr) {
			return;
		}
		uintptr_t soundID = (uintptr_t)source;

		// アンロードするモデルを検索
		auto it = std::find_if(resources.begin(), resources.end(), 
			[soundID](const std::pair<std::u16string, SoundResource>& pair){
				return pair.second.soundID == soundID;
			});

		// 参照カウンタが0になったら実際にアンロード
		it->second.referenceCount--;
		if (it->second.referenceCount <= 0) {
			unload( it->first.c_str() );
			resources.erase(it);
		}
	}
	
	SoundLoader* SoundLoader::Create(
		SoundLoaderLoad load,
		SoundLoaderUnload unload)
	{
		return new SoundLoader(load, unload);
	}

	SoundPlayer* SoundPlayer::Create(
		SoundPlayerPlay play,
		SoundPlayerStopTag stopTag,
		SoundPlayerPauseTag pauseTag,
		SoundPlayerCheckPlayingTag checkPlayingTag,
		SoundPlayerStopAll stopAll)
	{
		return new SoundPlayer(play, stopTag, pauseTag, checkPlayingTag, stopAll);
	}
}
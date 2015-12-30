#include "EffekseerPluginSound.h"

namespace EffekseerPlugin
{
	void* SoundLoader::Load( const EFK_CHAR* path ){
		auto it = resources.find((const char16_t*)path);
		if (it != resources.end()) {
			it->second.referenceCount++;
			return (void*)it->second.soundID;
		} else {
			SoundResource res;
			res.soundID = load( (const char16_t*)path );
			if (res.soundID <= 0) {
				return nullptr;
			}
			resources.insert( std::make_pair(
				(const char16_t*)path, res ) );
			return (void*)res.soundID;
		}
	}

	void SoundLoader::Unload( void* source ){
		int soundID = (int)source;
		for (auto it = resources.begin(); it != resources.end(); it++) {
			if (it->second.soundID == soundID) {
				it->second.referenceCount--;
				if (it->second.referenceCount <= 0) {
					unload( it->first.c_str() );
					resources.erase(it);
				}
			}
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
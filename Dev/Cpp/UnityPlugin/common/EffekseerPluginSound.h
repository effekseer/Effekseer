
#ifndef	__EFFEKSEER_PLUGIN_SOUND_H__
#define __EFFEKSEER_PLUGIN_SOUND_H__

#include <string>
#include <map>
#include <Effekseer.h>
#include "EffekseerPluginCommon.h"

namespace EffekseerPlugin
{
	using SoundTag = Effekseer::SoundTag;
	using SoundHandle = Effekseer::SoundHandle;

	using SoundLoaderLoad = int (UNITY_API*)(const char16_t* path);
	using SoundLoaderUnload = void (UNITY_API*)(const char16_t* path);

	class SoundLoader : public Effekseer::SoundLoader
	{
		SoundLoaderLoad load;
		SoundLoaderUnload unload;
		
		struct SoundResource {
			int referenceCount = 1;
			uintptr_t soundID = 0;
		};
		std::map<std::u16string, SoundResource> resources;

	public:
		static SoundLoader* Create(
			SoundLoaderLoad load,
			SoundLoaderUnload unload);
		SoundLoader(
			SoundLoaderLoad load,
			SoundLoaderUnload unload) 
			: load(load), unload(unload) {}
		virtual ~SoundLoader() {}
		virtual void* Load( const EFK_CHAR* path );
		virtual void Unload( void* source );
	};

	using SoundPlayerPlay = void (UNITY_API*)( SoundTag tag, 
		int Data, float Volume, float Pan, float Pitch, 
		bool Mode3D, float x, float y, float z, float Distance );
	using SoundPlayerStopTag = void (UNITY_API*)( SoundTag tag );
	using SoundPlayerPauseTag = void (UNITY_API*)( SoundTag tag, bool pause );
	using SoundPlayerCheckPlayingTag = bool (UNITY_API*)( SoundTag tag );
	using SoundPlayerStopAll = void (UNITY_API*)();

	class SoundPlayer : public Effekseer::SoundPlayer
	{
		SoundPlayerPlay play;
		SoundPlayerStopTag stopTag;
		SoundPlayerPauseTag pauseTag;
		SoundPlayerCheckPlayingTag checkPlayingTag;
		SoundPlayerStopAll stopAll;
	
	public:
		static SoundPlayer* Create(
			SoundPlayerPlay play,
			SoundPlayerStopTag stopTag,
			SoundPlayerPauseTag pauseTag,
			SoundPlayerCheckPlayingTag checkPlayingTag,
			SoundPlayerStopAll stopAll);
		SoundPlayer(
			SoundPlayerPlay play,
			SoundPlayerStopTag stopTag,
			SoundPlayerPauseTag pauseTag,
			SoundPlayerCheckPlayingTag checkPlayingTag,
			SoundPlayerStopAll stopAll
		): play(play), stopTag(stopTag), pauseTag(pauseTag), 
			checkPlayingTag(checkPlayingTag), stopAll(stopAll) {}
		virtual ~SoundPlayer() {}
		virtual SoundHandle Play( SoundTag tag, const InstanceParameter& parameter ){
			play( tag, (int)(uintptr_t)parameter.Data, parameter.Volume, parameter.Pan, parameter.Pitch, 
				parameter.Mode3D, parameter.Position.X, parameter.Position.Y, 
				parameter.Position.Z, parameter.Distance );
			return 0;
		}
		virtual void Stop( SoundHandle handle, SoundTag tag ){
		}
		virtual void Pause( SoundHandle handle, SoundTag tag, bool pause ){
		}
		virtual bool CheckPlaying( SoundHandle handle, SoundTag tag ){
			return false;
		}
		virtual void StopTag( SoundTag tag ){
			stopTag( tag );
		}
		virtual void PauseTag( SoundTag tag, bool pause ){
			pauseTag( tag, pause );
		}
		virtual bool CheckPlayingTag( SoundTag tag ){
			return checkPlayingTag( tag );
		}
		virtual void StopAll(){
			stopAll();
		}
	};
}

#endif

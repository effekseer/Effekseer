
#ifndef	__EFFEKSEER_SOUND_PLAYER_H__
#define	__EFFEKSEER_SOUND_PLAYER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../Effekseer.Base.h"
#include "../Effekseer.Vector2D.h"
#include "../Effekseer.Vector3D.h"
#include "../Effekseer.SoundLoader.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

typedef void* SoundHandle;
typedef void* SoundTag;

class SoundPlayer
{
public:
	struct InstanceParameter
	{
		void*		Data;
		float		Volume;
		float		Pan;
		float		Pitch;
		bool		Mode3D;
		Vector3D	Position;
		float		Distance;
	};

public:
	SoundPlayer() {}

	virtual ~SoundPlayer() {}

	virtual SoundHandle Play( SoundTag tag, const InstanceParameter& parameter ) = 0;
	
	virtual void Stop( SoundHandle handle, SoundTag tag ) = 0;

	virtual void Pause( SoundHandle handle, SoundTag tag, bool pause ) = 0;

	virtual bool CheckPlaying( SoundHandle handle, SoundTag tag ) = 0;

	virtual void StopTag( SoundTag tag ) = 0;

	virtual void PauseTag( SoundTag tag, bool pause ) = 0;

	virtual bool CheckPlayingTag( SoundTag tag ) = 0;

	virtual void StopAll() = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_SOUND_PLAYER_H__

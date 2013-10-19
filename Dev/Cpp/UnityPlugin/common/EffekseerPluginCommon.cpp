
#include "UnityPluginInterface.h"
#include "Effekseer.h"

extern Effekseer::Manager*				g_EffekseerManager;

extern "C"
{
	int EXPORT_API EffekseerPlayEffect(Effekseer::Effect* effect, float x, float y, float z)
	{
		if (effect != NULL) {
			return g_EffekseerManager->Play(effect, x, y, z);
		}
		return -1;
	}

	void EXPORT_API EffekseerStopEffect(int handle)
	{
		return g_EffekseerManager->StopEffect(handle);
	}

	void EXPORT_API EffekseerStopAllEffects()
	{
		return g_EffekseerManager->StopAllEffects();
	}
	
	void EXPORT_API EffekseerSetShown(int handle, int shown)
	{
		g_EffekseerManager->SetShown(handle, shown != 0);
	}
	
	void EXPORT_API EffekseerSetPaused(int handle, int paused)
	{
		g_EffekseerManager->SetPaused(handle, paused != 0);
	}
	
	int EXPORT_API EffekseerExists(int handle)
	{
		return g_EffekseerManager->Exists(handle);
	}

	void EXPORT_API EffekseerSetLocation(int handle, float x, float y, float z)
	{
		g_EffekseerManager->SetLocation(handle, x, y, z);
	}

	void EXPORT_API EffekseerSetRotation(int handle, float x, float y, float z, float angle)
	{
		Effekseer::Vector3D axis(x, y, z);
		g_EffekseerManager->SetRotation(handle, axis, angle);
	}

	void EXPORT_API EffekseerSetScale(int handle, float x, float y, float z)
	{
		g_EffekseerManager->SetScale(handle, x, y, z);
	}
}

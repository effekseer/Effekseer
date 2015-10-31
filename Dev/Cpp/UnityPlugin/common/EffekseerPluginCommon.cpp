#include "EffekseerPluginCommon.h"
#include "Effekseer.h"

Effekseer::Matrix44				g_cameraMatrix[MAX_RENDER_PATH];
Effekseer::Matrix44				g_projectionMatrix[MAX_RENDER_PATH];

static void Array2Matrix(Effekseer::Matrix44& matrix, float matrixArray[])
{
	matrix.Values[0][0] = matrixArray[ 0];
	matrix.Values[1][0] = matrixArray[ 1];
	matrix.Values[2][0] = matrixArray[ 2];
	matrix.Values[3][0] = matrixArray[ 3];
	matrix.Values[0][1] = matrixArray[ 4];
	matrix.Values[1][1] = matrixArray[ 5];
	matrix.Values[2][1] = matrixArray[ 6];
	matrix.Values[3][1] = matrixArray[ 7];
	matrix.Values[0][2] = matrixArray[ 8];
	matrix.Values[1][2] = matrixArray[ 9];
	matrix.Values[2][2] = matrixArray[10];
	matrix.Values[3][2] = matrixArray[11];
	matrix.Values[0][3] = matrixArray[12];
	matrix.Values[1][3] = matrixArray[13];
	matrix.Values[2][3] = matrixArray[14];
	matrix.Values[3][3] = matrixArray[15];
}

extern "C"
{
	DLLEXPORT void UNITY_API EffekseerUpdate(float deltaFrame)
	{
		if (g_EffekseerManager == NULL) {
			return;
		}
		
		g_EffekseerManager->Update(deltaFrame);
	}

	DLLEXPORT Effekseer::Effect* UNITY_API EffekseerLoadEffect(const EFK_CHAR* path)
	{
		if (g_EffekseerManager == NULL) {
			return NULL;
		}
		
		return Effekseer::Effect::Create(g_EffekseerManager, path);
	}

	DLLEXPORT void UNITY_API EffekseerReleaseEffect(Effekseer::Effect* effect)
	{
		if (effect != NULL) {
			effect->Release();
		}
	}

	DLLEXPORT int UNITY_API EffekseerPlayEffect(Effekseer::Effect* effect, float x, float y, float z)
	{
		if (g_EffekseerManager == NULL) {
			return -1;
		}

		if (effect != NULL) {
			return g_EffekseerManager->Play(effect, x, y, z);
		}
		return -1;
	}

	DLLEXPORT void UNITY_API EffekseerStopEffect(int handle)
	{
		if (g_EffekseerManager == NULL) {
			return;
		}

		g_EffekseerManager->StopEffect(handle);
	}

	DLLEXPORT void UNITY_API EffekseerStopAllEffects()
	{
		if (g_EffekseerManager == NULL) {
			return;
		}

		g_EffekseerManager->StopAllEffects();
	}
	
	DLLEXPORT void UNITY_API EffekseerSetShown(int handle, int shown)
	{
		if (g_EffekseerManager == NULL) {
			return;
		}

		g_EffekseerManager->SetShown(handle, shown != 0);
	}
	
	DLLEXPORT void UNITY_API EffekseerSetPaused(int handle, int paused)
	{
		if (g_EffekseerManager == NULL) {
			return;
		}

		g_EffekseerManager->SetPaused(handle, paused != 0);
	}
	
	DLLEXPORT int UNITY_API EffekseerExists(int handle)
	{
		if (g_EffekseerManager == NULL) {
			return false;
		}

		return g_EffekseerManager->Exists(handle);
	}

	DLLEXPORT void UNITY_API EffekseerSetLocation(int handle, float x, float y, float z)
	{
		if (g_EffekseerManager == NULL) {
			return;
		}

		g_EffekseerManager->SetLocation(handle, x, y, z);
	}

	DLLEXPORT void UNITY_API EffekseerSetRotation(int handle, float x, float y, float z, float angle)
	{
		if (g_EffekseerManager == NULL) {
			return;
		}

		Effekseer::Vector3D axis(x, y, z);
		g_EffekseerManager->SetRotation(handle, axis, angle);
	}

	DLLEXPORT void UNITY_API EffekseerSetScale(int handle, float x, float y, float z)
	{
		if (g_EffekseerManager == NULL) {
			return;
		}

		g_EffekseerManager->SetScale(handle, x, y, z);
	}
	
	DLLEXPORT void UNITY_API EffekseerSetProjectionMatrix(int renderId, float matrixArray[])
	{
		if (renderId >= 0 && renderId < MAX_RENDER_PATH) {
			Effekseer::Matrix44& matrix = g_projectionMatrix[renderId];
			Array2Matrix(matrix, matrixArray);
		}
	}

	DLLEXPORT void UNITY_API EffekseerSetCameraMatrix(int renderId, float matrixArray[])
	{
		if (renderId >= 0 && renderId < MAX_RENDER_PATH) {
			Effekseer::Matrix44& matrix = g_cameraMatrix[renderId];
			Array2Matrix(matrix, matrixArray);
		}
	}
}

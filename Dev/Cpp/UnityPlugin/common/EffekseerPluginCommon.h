
#ifndef	__EFFEKSEER_PLUGIN_COMMON_H__
#define __EFFEKSEER_PLUGIN_COMMON_H__

#include <vector>
#include "Effekseer.h"
#include "IUnityInterface.h"

#define DLLEXPORT UNITY_INTERFACE_EXPORT
#define UNITY_API UNITY_INTERFACE_API

namespace EffekseerPlugin
{
	const int MAX_RENDER_PATH = 128;

	extern Effekseer::Manager*	g_EffekseerManager;

	struct RenderSettings {
		Effekseer::Matrix44		cameraMatrix;
		Effekseer::Matrix44		projectionMatrix;
		bool					renderIntoTexture;
	};
	extern RenderSettings renderSettings[MAX_RENDER_PATH];

	void Array2Matrix(Effekseer::Matrix44& matrix, float matrixArray[]);
}

#endif

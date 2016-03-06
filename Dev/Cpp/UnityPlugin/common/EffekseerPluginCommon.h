
#ifndef	__EFFEKSEER_PLUGIN_COMMON_H__
#define __EFFEKSEER_PLUGIN_COMMON_H__

#include <vector>
#include "Effekseer.h"
#include "IUnityInterface.h"

#define DLLEXPORT UNITY_INTERFACE_EXPORT
#define UNITY_API UNITY_INTERFACE_API

extern Effekseer::Manager*				g_EffekseerManager;

const int MAX_RENDER_PATH = 128;
extern Effekseer::Matrix44	g_cameraMatrix[MAX_RENDER_PATH];
extern Effekseer::Matrix44	g_projectionMatrix[MAX_RENDER_PATH];

void Array2Matrix(Effekseer::Matrix44& matrix, float matrixArray[]);
int32_t EfkStrLen(const EFK_CHAR *str);

#endif

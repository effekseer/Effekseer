
#include "../common/UnityPluginInterface.h"
#include <OpenGL/OpenGL.h>
#include <OpenAl/al.h>
#include <OpenAl/alc.h>

#include <Effekseer.h>
#include <EffekseerRendererGL.h>
#include <EffekseerSoundAL.h>

static const int RENDER_EVENT_ID	= 0x2040;

static int						g_DeviceType = -1;

Effekseer::Manager*				g_EffekseerManager = NULL;
EffekseerRenderer::Renderer*	g_EffekseerRenderer = NULL;
EffekseerSound::Sound*			g_EffekseerSound = NULL;
static ALCdevice*				g_alcdev = NULL;
static ALCcontext*				g_alcctx = NULL;


static void InitializeOpenAL();
static void FinalizeOpenAL();
static void InitializeEffekseer();
static void FinalizeEffekseer();

static void InitializeOpenAL()
{
	g_alcdev = alcOpenDevice(NULL);
	g_alcctx = alcCreateContext(g_alcdev, NULL);
	alcMakeContextCurrent(g_alcctx);
}

static void FinalizeOpenAL()
{
	if (g_alcctx) {
		alcDestroyContext(g_alcctx);
		g_alcctx = NULL;
	}
	if (g_alcdev) {
		alcCloseDevice(g_alcdev);
		g_alcdev = NULL;
	}
}

static void InitializeEffekseer(int maxInstances, int maxSquares)
{
	g_EffekseerManager = Effekseer::Manager::Create(maxInstances);

	g_EffekseerRenderer = EffekseerRendererGL::Renderer::Create(maxSquares);
	g_EffekseerManager->SetSpriteRenderer(g_EffekseerRenderer->CreateSpriteRenderer());
	g_EffekseerManager->SetRibbonRenderer(g_EffekseerRenderer->CreateRibbonRenderer());
	g_EffekseerManager->SetRingRenderer(g_EffekseerRenderer->CreateRingRenderer());
	g_EffekseerManager->SetModelRenderer(g_EffekseerRenderer->CreateModelRenderer());

	g_EffekseerManager->SetTextureLoader(g_EffekseerRenderer->CreateTextureLoader());
	g_EffekseerManager->SetModelLoader(g_EffekseerRenderer->CreateModelLoader());

	InitializeOpenAL();
	
	g_EffekseerSound = EffekseerSound::Sound::Create(32);
	g_EffekseerManager->SetSoundPlayer(g_EffekseerSound->CreateSoundPlayer());
	g_EffekseerManager->SetSoundLoader(g_EffekseerSound->CreateSoundLoader());
}

static void FinalizeEffekseer()
{
	if (g_EffekseerManager != NULL) {
		g_EffekseerManager->Destroy();
		g_EffekseerManager = NULL;
	}
	if (g_EffekseerSound != NULL) {
		g_EffekseerSound->Destory();
		g_EffekseerSound = NULL;
	}
	FinalizeOpenAL();
	
	if (g_EffekseerRenderer != NULL) {
		g_EffekseerRenderer->Destory();
		g_EffekseerRenderer = NULL;
	}
}

static void SetGraphicsDeviceGL(GfxDeviceEventType eventType)
{
	switch (eventType) {
	case kGfxDeviceEventInitialize:
	case kGfxDeviceEventAfterReset:
		break;
	case kGfxDeviceEventBeforeReset:
	case kGfxDeviceEventShutdown:
		break;
	}
}

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
	void EXPORT_API UnitySetGraphicsDevice(void* device, int deviceType, int eventType)
	{
		g_DeviceType = -1;

		if (deviceType == kGfxRendererOpenGL) {
			g_DeviceType = deviceType;
			SetGraphicsDeviceGL((GfxDeviceEventType)eventType);
		}
	}

	void EXPORT_API UnityRenderEvent(int eventID)
	{
		if (g_DeviceType == -1) {
			return;
		}
		if (g_EffekseerManager == NULL || g_EffekseerRenderer == NULL) {
			return;
		}
		if (eventID != RENDER_EVENT_ID) {
			return;
		}
	
		g_EffekseerRenderer->BeginRendering();
		g_EffekseerManager->Draw();
		g_EffekseerRenderer->EndRendering();
	}

	void EXPORT_API EffekseerInit(int maxInstances, int maxSquares)
	{
		InitializeEffekseer(maxInstances, maxSquares);
	}

	void EXPORT_API EffekseerTerm()
	{
		FinalizeEffekseer();
	}

	void EXPORT_API EffekseerSetProjectionMatrix(float matrixArray[])
	{
		if (g_EffekseerRenderer == NULL) {
			return;
		}

		Effekseer::Matrix44 matrix;
		Array2Matrix(matrix, matrixArray);
		g_EffekseerRenderer->SetProjectionMatrix(matrix);
	}

	void EXPORT_API EffekseerSetCameraMatrix(float matrixArray[])
	{
		if (g_EffekseerRenderer == NULL) {
			return;
		}

		Effekseer::Matrix44 matrix;
		Array2Matrix(matrix, matrixArray);
		g_EffekseerRenderer->SetCameraMatrix(matrix);
	}
}
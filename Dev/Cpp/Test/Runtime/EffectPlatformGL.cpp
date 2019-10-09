#include "EffectPlatformGL.h"

EffekseerRenderer::Renderer* EffectPlatformGL::CreateRenderer() { return nullptr; }

EffectPlatformGL::~EffectPlatformGL() {}

void EffectPlatformGL::InitializeDevice(const EffectPlatformInitializingParameter& param) {}

void EffectPlatformGL::BeginRendering()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EffectPlatformGL::EndRendering() {}

void EffectPlatformGL::Present() {}


#ifndef __EFFEKSEERRENDERER_CULLING_H__
#define __EFFEKSEERRENDERER_CULLING_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../Graphics/efk.LineRenderer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Culling
{
private:
	efk::LineRenderer* lineRenderer = nullptr;
	Culling(efk::Graphics* graphics, EffekseerRenderer::Renderer* renderer);

public:
	virtual ~Culling();

	static Culling* Create(efk::Graphics* graphics, EffekseerRenderer::Renderer* renderer);

public:
	void Rendering(bool isRightHand);

	bool IsShown;
	float Radius;
	float X;
	float Y;
	float Z;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_CULLING_H__
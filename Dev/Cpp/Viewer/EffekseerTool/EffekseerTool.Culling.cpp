
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.Shader.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RenderState.h>
#include "EffekseerTool.Culling.h"


namespace EffekseerRenderer
{

Culling::Culling( EffekseerRendererDX9::RendererImplemented* renderer)
	: DeviceObject( renderer )
	, m_renderer	( renderer )
	, IsShown		(false)
	, Radius		(0.0f)
	, X				(0.0f)
	, Y				(0.0f)
	, Z				(0.0f)
{
	lineRenderer = new efk::LineRendererDX9(renderer);
}

Culling::~Culling()
{
	ES_SAFE_DELETE(lineRenderer);
}

Culling* Culling::Create( EffekseerRendererDX9::RendererImplemented* renderer )
{
	assert(renderer != NULL);
	assert(renderer->GetDevice() != NULL);

	return new Culling( renderer );
}

void Culling::OnLostDevice()
{
	lineRenderer->OnLostDevice();
}

void Culling::OnResetDevice()
{
	lineRenderer->OnResetDevice();
}

void Culling::Rendering(bool isRightHand )
{
	if(!IsShown) return;

	lineRenderer->ClearCache();

	for (int32_t y = -3; y <= 3; y++)
	{
		float ylen = Radius * ((float)y / 4.0f);
		float radius = sqrt(Radius * Radius - ylen * ylen);

		for (int32_t r = 0; r < 9; r++)
		{
			float a0 = 3.1415f * 2.0f / 9.0f * r;
			float a1 = 3.1415f * 2.0f / 9.0f * (r + 1.0f);

			lineRenderer->DrawLine(
				::Effekseer::Vector3D(X + sin(a0) * radius, Y + ylen, Z + cos(a0) * radius),
				::Effekseer::Vector3D(X + sin(a1) * radius, Y + ylen, Z + cos(a1) * radius),
				::Effekseer::Color(255, 255, 255, 255));
		}
	}

	lineRenderer->Render();
}

}


#include "EffekseerTool.Paste.h"

namespace EffekseerRenderer
{

Paste::Paste(efk::Graphics* graphics)
{
	imageRenderer = efk::ImageRenderer::Create(graphics);
}

Paste::~Paste()
{
	ES_SAFE_DELETE(imageRenderer);
}

Paste* Paste::Create(efk::Graphics* graphics)
{
	return new Paste(graphics);
}

void Paste::Rendering(::Effekseer::TextureData* texture, int32_t width, int32_t height)
{
	if (texture == nullptr) return;

	Effekseer::Vector3D pos[4];
	Effekseer::Vector2D uv[4];

	auto offset_x = 0.5f / (float)width;
	auto offset_y = 0.5f / (float)height;

	pos[0].X = -1.0f;
	pos[0].Y = -1.0f;
	pos[0].Z = 0.5f;
	uv[0].X = 0.0f + offset_x;
	uv[0].Y = 1.0f + offset_y;

	pos[1].X = -1.0f;
	pos[1].Y = 1.0f;
	pos[1].Z = 0.5f;
	uv[1].X = 0.0f + offset_x;
	uv[1].Y = 0.0f + offset_y;

	pos[2].X = 1.0f;
	pos[2].Y = -1.0f;
	pos[2].Z = 0.5f;
	uv[2].X = 1.0f + offset_x;
	uv[2].Y = 1.0f + offset_y;

	pos[3].X = 1.0f;
	pos[3].Y = 1.0f;
	pos[3].Z = 0.5f;
	uv[3].X = 1.0f + offset_x;
	uv[3].Y = 0.0f + offset_y;

	Effekseer::Color colors[4];
	for (auto i = 0; i < 4; i++)
	{
		colors[i] = Effekseer::Color(255, 255, 255, 255);
	}

	imageRenderer->ClearCache();
	imageRenderer->Draw(pos, uv, colors, texture);
	imageRenderer->Render();
}

}


#include "EffekseerTool.Guide.h"

namespace EffekseerRenderer
{

Guide::Guide(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
{
	imageRenderer_ = Effekseer::Tool::ImageRenderer::Create(graphicsDevice);
}

Guide::~Guide()
{
}

Guide* Guide::Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
{
	return new Guide(graphicsDevice);
}

void Guide::Rendering(int32_t width, int32_t height, int32_t guide_width, int32_t guide_height)
{
	imageRenderer_->ClearCache();

	float ul_x = 0;
	float ul_y = 0;
	float ul_gx = (width - guide_width) / 2.0f;
	float ul_gy = (height - guide_height) / 2.0f;

	float ur_x = ul_x + width;
	float ur_y = ul_y;
	float ur_gx = ul_gx + guide_width;
	float ur_gy = ul_gy;

	float dl_x = ul_x;
	float dl_y = ul_y + height;
	float dl_gx = ul_gx;
	float dl_gy = ul_gy + guide_height;

	float dr_x = ur_x;
	float dr_y = dl_y;
	float dr_gx = ur_gx;
	float dr_gy = dl_gy;

	Effekseer::Vector2D uv[4];
	Effekseer::Color colors[4];

	for (int32_t i = 0; i < 4; i++)
	{
		uv[i].X = 0.0f;
		uv[i].Y = 0.0f;
		colors[i].R = 255;
		colors[i].G = 0;
		colors[i].B = 0;
		colors[i].A = 32;
	}

	{
		Effekseer::Vector3D pos[4];

		pos[0].X = ul_gx;
		pos[0].Y = ul_gy;
		pos[1].X = ul_x;
		pos[1].Y = ul_y;
		pos[3].X = ur_x;
		pos[3].Y = ur_y;
		pos[2].X = ur_gx;
		pos[2].Y = ur_gy;
		pos[0].Z = 0.5f;
		pos[1].Z = 0.5f;
		pos[2].Z = 0.5f;
		pos[3].Z = 0.5f;

		for (int32_t i = 0; i < 4; i++)
		{
			pos[i].X = pos[i].X / (float)width * 2.0f - 1.0f;
			pos[i].Y = 1.0f - pos[i].Y / (float)height * 2.0f;
		}

		imageRenderer_->Draw(pos, uv, colors, nullptr);
	}

	{
		Effekseer::Vector3D pos[4];

		pos[0].X = ul_x;
		pos[0].Y = ul_y;
		pos[1].X = ul_gx;
		pos[1].Y = ul_gy;
		pos[3].X = dl_gx;
		pos[3].Y = dl_gy;
		pos[2].X = dl_x;
		pos[2].Y = dl_y;
		pos[0].Z = 0.5f;
		pos[1].Z = 0.5f;
		pos[2].Z = 0.5f;
		pos[3].Z = 0.5f;

		for (int32_t i = 0; i < 4; i++)
		{
			pos[i].X = pos[i].X / (float)width * 2.0f - 1.0f;
			pos[i].Y = 1.0f - pos[i].Y / (float)height * 2.0f;
		}

		imageRenderer_->Draw(pos, uv, colors, nullptr);
	}

	{
		Effekseer::Vector3D pos[4];

		pos[0].X = dl_x;
		pos[0].Y = dl_y;
		pos[1].X = dl_gx;
		pos[1].Y = dl_gy;
		pos[3].X = dr_gx;
		pos[3].Y = dr_gy;
		pos[2].X = dr_x;
		pos[2].Y = dr_y;
		pos[0].Z = 0.5f;
		pos[1].Z = 0.5f;
		pos[2].Z = 0.5f;
		pos[3].Z = 0.5f;

		for (int32_t i = 0; i < 4; i++)
		{
			pos[i].X = pos[i].X / (float)width * 2.0f - 1.0f;
			pos[i].Y = 1.0f - pos[i].Y / (float)height * 2.0f;
		}

		imageRenderer_->Draw(pos, uv, colors, nullptr);
	}

	{
		Effekseer::Vector3D pos[4];

		pos[0].X = ur_x;
		pos[0].Y = ur_y;
		pos[1].X = dr_x;
		pos[1].Y = dr_y;
		pos[3].X = dr_gx;
		pos[3].Y = dr_gy;
		pos[2].X = ur_gx;
		pos[2].Y = ur_gy;
		pos[0].Z = 0.5f;
		pos[1].Z = 0.5f;
		pos[2].Z = 0.5f;
		pos[3].Z = 0.5f;

		for (int32_t i = 0; i < 4; i++)
		{
			pos[i].X = pos[i].X / (float)width * 2.0f - 1.0f;
			pos[i].Y = 1.0f - pos[i].Y / (float)height * 2.0f;
		}

		imageRenderer_->Draw(pos, uv, colors, nullptr);
	}

	imageRenderer_->Render();
}

} // namespace EffekseerRenderer

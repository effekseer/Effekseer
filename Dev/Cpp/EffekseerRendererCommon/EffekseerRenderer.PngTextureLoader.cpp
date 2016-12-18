
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.PngTextureLoader.h"

#if _WIN32
#define __PNG_DDI 1
#else
#define __PNG_DDI 0
#endif

#if __PNG_DDI
#define WINVER          0x0501
#define _WIN32_WINNT    0x0501
#include <windows.h>
#include <gdiplus.h>
#else
#include <png.h>
#endif

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
#if __PNG_DDI
static Gdiplus::GdiplusStartupInput		gdiplusStartupInput;
static ULONG_PTR						gdiplusToken;
#else
static void PngReadData(png_structp png_ptr, png_bytep data, png_size_t length)
{
	uint8_t** d = (uint8_t**) png_get_io_ptr(png_ptr);
	memcpy(data, *d, length);
	(*d) += length;
}
#endif

std::vector<uint8_t> PngTextureLoader::textureData;
int32_t PngTextureLoader::textureWidth;
int32_t PngTextureLoader::textureHeight;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool PngTextureLoader::Load(void* data, int32_t size, bool rev)
{
#if __PNG_DDI
	auto global = GlobalAlloc(GMEM_MOVEABLE,size);
	auto buf = GlobalLock(global);
	CopyMemory(buf, data, size);
	GlobalUnlock(global);
	LPSTREAM stream = NULL;
	CreateStreamOnHGlobal( global, false, &stream);
	Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromStream(stream);
	ES_SAFE_RELEASE(stream);
	GlobalFree(global);


	if( bmp != NULL && bmp->GetLastStatus() == Gdiplus::Ok )
	{
		textureWidth = bmp->GetWidth();
		textureHeight = bmp->GetHeight();
		textureData.resize(textureWidth * textureHeight * 4);

		if(rev)
		{
			for(auto y = 0; y < textureHeight; y++ )
			{
				for(auto x = 0; x < textureWidth; x++ )
				{
					Gdiplus::Color  color;
					bmp->GetPixel(x, textureHeight - y - 1, &color);

					textureData[(x + y * textureWidth) * 4 + 0] = color.GetR();
					textureData[(x + y * textureWidth) * 4 + 1] = color.GetG();
					textureData[(x + y * textureWidth) * 4 + 2] = color.GetB();
					textureData[(x + y * textureWidth) * 4 + 3] = color.GetA();
				}
			}
		}
		else
		{
			for(auto y = 0; y < textureHeight; y++ )
			{
				for(auto x = 0; x < textureWidth; x++ )
				{
					Gdiplus::Color  color;
					bmp->GetPixel(x, y, &color);

					textureData[(x + y * textureWidth) * 4 + 0] = color.GetR();
					textureData[(x + y * textureWidth) * 4 + 1] = color.GetG();
					textureData[(x + y * textureWidth) * 4 + 2] = color.GetB();
					textureData[(x + y * textureWidth) * 4 + 3] = color.GetA();
				}
			}
		}
		
		ES_SAFE_DELETE(bmp);
		return true;
	}
	else
	{
		ES_SAFE_DELETE(bmp);
		return false;
	}
	
	ES_SAFE_DELETE(bmp);

#elif PNG_LIBPNG_VER < 10504 /* Libpng 1.2系 */
	uint8_t* data_ = (uint8_t*) data;

	/* pngアクセス構造体を作成 */
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	/* リードコールバック関数指定 */
	png_set_read_fn(png, &data_, &PngReadData);

	/* png画像情報構造体を作成 */
	png_infop png_info = png_create_info_struct(png);

	/* エラーハンドリング */
	if (setjmp(png_jmpbuf(png)))
	{
		png_destroy_read_struct(&png, &png_info, NULL);
		return false;
	}

	/* IHDRチャンク情報を取得 */
	png_read_info(png, png_info);

	/* RGBA8888フォーマットに変換する */
	if (png_info->bit_depth < 8)
	{
		png_set_packing(png);
	}
	else if (png_info->bit_depth == 16)
	{
		png_set_strip_16(png);
	}

	uint32_t pixelBytes = 4;
	switch (png_info->color_type)
	{
	case PNG_COLOR_TYPE_PALETTE:
		png_set_palette_to_rgb(png);
		pixelBytes = 4;
		break;
	case PNG_COLOR_TYPE_GRAY:
		png_set_expand_gray_1_2_4_to_8(png);
		pixelBytes = 3;
		break;
	case PNG_COLOR_TYPE_RGB:
		pixelBytes = 3;
		break;
	case PNG_COLOR_TYPE_RGBA:
		break;
	}

	uint8_t* image = new uint8_t[png_info->width * png_info->height * pixelBytes];
	uint32_t pitch = png_info->width * pixelBytes;

	/* イメージデータを読み込む */

	textureWidth = png_info->width;
	textureHeight = png_info->height;
	textureData.resize(textureWidth * textureHeight * 4);

	if (rev)
	{
		for (uint32_t i = 0; i < png_info->height; i++)
		{
			png_read_row(png, &image[(png_info->height - 1 - i) * pitch], NULL);
		}
	}
	else
	{
		for (uint32_t i = 0; i < png_info->height; i++)
		{
			png_read_row(png, &image[i * pitch], NULL);
		}
	}

	if (pixelBytes == 4)
	{
		memcpy(textureData.data(), image, png_info->width * png_info->height * pixelBytes);
	}
	else
	{
		for (int32_t y = 0; y < png_info->height; y++)
		{
			for (int32_t x = 0; x < png_info->width; x++)
			{
				int32_t src = (x + y * png_info->width) * 3;
				int32_t dst = (x + y * png_info->width) * 4;
				textureData[dst + 0] = image[src + 0];
				textureData[dst + 1] = image[src + 1];
				textureData[dst + 2] = image[src + 2];
				textureData[dst + 3] = 255;
			}
		}
	}
	
	delete [] image;
	png_destroy_read_struct(&png, &png_info, NULL);

	return true;
#else /* Libpng 1.6系 */
	textureWidth = 0;
	textureHeight = 0;
	textureData.clear();

	uint8_t* data_ = (uint8_t*) data;

	/* pngアクセス構造体を作成 */
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	/* リードコールバック関数指定 */
	png_set_read_fn(png, &data_, &PngReadData);

	/* png画像情報構造体を作成 */
	png_infop png_info = png_create_info_struct(png);

	/* エラーハンドリング */
	if (setjmp(png_jmpbuf(png)))
	{
		png_destroy_read_struct(&png, &png_info, NULL);

		// if (log != nullptr)
		// {
		// 	log->WriteLineStrongly("pngファイルのヘッダの読み込みに失敗しました。");
		// }
		
		return false;
	}

	/* IHDRチャンク情報を取得 */
	png_read_info(png, png_info);

	/* RGBA8888フォーマットに変換する */
	const png_byte bit_depth = png_get_bit_depth(png, png_info);
	if (bit_depth < 8)
	{
		png_set_packing(png);
	}
	else if (bit_depth == 16)
	{
		png_set_strip_16(png);
	}

	uint32_t pixelBytes = 4;
	const png_byte color_type = png_get_color_type(png, png_info);
	switch (color_type)
	{
	case PNG_COLOR_TYPE_PALETTE:
		{
			png_set_palette_to_rgb(png);

			png_bytep trans_alpha = NULL;
			int num_trans = 0;
			png_color_16p trans_color = NULL;

			png_get_tRNS(png, png_info, &trans_alpha, &num_trans, &trans_color);
			if (trans_alpha != NULL)
			{
				pixelBytes = 4;
			}
			else
			{
				pixelBytes = 3;
			}
		}
		break;
	case PNG_COLOR_TYPE_GRAY:
		png_set_expand_gray_1_2_4_to_8(png);
		pixelBytes = 3;
		break;
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		png_set_gray_to_rgb(png);
		pixelBytes = 4;
		break;
	case PNG_COLOR_TYPE_RGB:
		pixelBytes = 3;
		break;
	case PNG_COLOR_TYPE_RGBA:
		break;
	}

	textureWidth = png_get_image_width(png, png_info);
	textureHeight = png_get_image_height(png, png_info);

	uint8_t* image = new uint8_t[textureWidth * textureHeight * pixelBytes];
	uint32_t pitch = textureWidth * pixelBytes;

	// 読み込み
	if (rev)
	{
		for (uint32_t i = 0; i < textureHeight; i++)
		{
			png_read_row(png, &image[(textureHeight - 1 - i) * pitch], NULL);
		}
	}
	else
	{
		for (uint32_t i = 0; i < textureHeight; i++)
		{
			png_read_row(png, &image[i * pitch], NULL);
		}
	}

	textureData.resize(textureWidth * textureHeight * 4);
	auto imagedst_ = textureData.data();

	if (pixelBytes == 4)
	{
		memcpy(imagedst_, image, textureWidth * textureHeight * 4);
	}
	else
	{
		for (int32_t y = 0; y < textureHeight; y++)
		{
			for (int32_t x = 0; x < textureWidth; x++)
			{
				auto src = (x + y * textureWidth) * 3;
				auto dst = (x + y * textureWidth) * 4;
				imagedst_[dst + 0] = image[src + 0];
				imagedst_[dst + 1] = image[src + 1];
				imagedst_[dst + 2] = image[src + 2];
				imagedst_[dst + 3] = 255;
			}
		}
	}

	delete[] image;
	png_destroy_read_struct(&png, &png_info, NULL);

	return true;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void PngTextureLoader::Unload()
{
	textureData.clear();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void PngTextureLoader::Initialize()
{
#if __PNG_DDI
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL );
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void PngTextureLoader::Finalize()
{
#if __PNG_DDI
	Gdiplus::GdiplusShutdown(gdiplusToken);
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

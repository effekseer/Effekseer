#include "efk.PNGHelper.h"

namespace efk
{
#ifdef _WIN32

#else
	void SavePNGImage(const char16_t* filepath, int32_t width, int32_t height, const void* data, bool rev)
	{
		/* 構造体確保 */
#if _WIN32
		FILE *fp = _wfopen(filepath, L"wb");
#else
		char filepath8[1024];
		Effekseer::ConvertUtf16ToUtf8((int8_t*)filepath8, sizeof(filepath8), (const int16_t*)filepath);
		FILE *fp = fopen(filepath8, "wb");
#endif

		if (fp == nullptr) return;

		png_structp pp = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		png_infop ip = png_create_info_struct(pp);

		/* 書き込み準備 */
		png_init_io(pp, fp);
		png_set_IHDR(pp, ip, width, height,
			8, /* 8bit以外にするなら変える */
			PNG_COLOR_TYPE_RGBA, /* RGBA以外にするなら変える */
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		/* ピクセル領域確保 */
		std::vector<png_byte>  raw1D(height * png_get_rowbytes(pp, ip));
		std::vector<png_bytep> raw2D(height * sizeof(png_bytep));
		for (int32_t i = 0; i < height; i++)
		{
			raw2D[i] = &raw1D[i*png_get_rowbytes(pp, ip)];
		}

		memcpy((void*)raw1D.data(), data, width * height * 4);

		/* 上下反転 */
		if (rev)
		{
			for (int32_t i = 0; i < height / 2; i++)
			{
				png_bytep swp = raw2D[i];
				raw2D[i] = raw2D[height - i - 1];
				raw2D[height - i - 1] = swp;
			}
		}

		/* 書き込み */
		png_write_info(pp, ip);
		png_write_image(pp, raw2D.data());
		png_write_end(pp, ip);

		/* 解放 */
		png_destroy_write_struct(&pp, &ip);
		fclose(fp);
	}
#endif

	PNGHelper::PNGHelper()
	{
#ifdef _WIN32
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		tempBuffer1.resize(2048 * 2048 * 4);
#endif
	}

	PNGHelper::~PNGHelper()
	{
#ifdef _WIN32
		Gdiplus::GdiplusShutdown(gdiplusToken);
#endif
	}

	bool PNGHelper::Save(const char16_t* path, int32_t width, int32_t height, const void* data)
	{
#ifdef _WIN32
		// Create bitmap data
		Gdiplus::Bitmap bmp(width, height);

		auto p = (Effekseer::Color*) data;
		for (int32_t y = 0; y < height; y++)
		{
			for (int32_t x = 0; x < width; x++)
			{
				auto src = p[x + width * y];
				Gdiplus::Color dst(src.A, src.R, src.G, src.B);
				bmp.SetPixel(x, y, dst);
			}
		}

		// Save bitmap
		CLSID id;
		UINT encoderNum = 0;
		UINT encoderSize = 0;
		Gdiplus::GetImageEncodersSize(&encoderNum, &encoderSize);
		if (encoderSize == 0)
		{
			return false;
		}

		auto imageCodecInfo = (Gdiplus::ImageCodecInfo*) malloc(encoderSize);
		Gdiplus::GetImageEncoders(encoderNum, encoderSize, imageCodecInfo);

		for (UINT i = 0; i < encoderNum; i++)
		{
			if (wcscmp(imageCodecInfo[i].MimeType, L"image/png") == 0)
			{
				id = imageCodecInfo[i].Clsid;
				free(imageCodecInfo);
				imageCodecInfo = nullptr;
				break;
			}
		}

		if (imageCodecInfo != nullptr)
		{
			free(imageCodecInfo);
			return false;
		}

		bmp.Save((const wchar_t*)path, &id);

		return true;
#else
		SavePNGImage(path, width, height, data, false);
		return true;
#endif
	}
}
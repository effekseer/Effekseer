#include "PNGHelper.h"

namespace efk
{

bool SavePNGImage(const char16_t* filepath, int32_t width, int32_t height, const void* data, bool rev)
{
#if _WIN32
	FILE* fp = _wfopen(reinterpret_cast<const wchar_t*>(filepath), L"wb");
#else
	char filepath8[1024];
	Effekseer::ConvertUtf16ToUtf8(filepath8, sizeof(filepath8), filepath);
	FILE* fp = fopen(filepath8, "wb");
#endif

	if (fp == nullptr)
		return false;

	png_structp pp = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop ip = png_create_info_struct(pp);

	png_init_io(pp, fp);
	png_set_IHDR(pp,
				 ip,
				 width,
				 height,
				 8,
				 PNG_COLOR_TYPE_RGBA,
				 PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_DEFAULT,
				 PNG_FILTER_TYPE_DEFAULT);

	std::vector<png_byte> raw1D(height * png_get_rowbytes(pp, ip));
	std::vector<png_bytep> raw2D(height * sizeof(png_bytep));
	for (int32_t i = 0; i < height; i++)
	{
		raw2D[i] = &raw1D[i * png_get_rowbytes(pp, ip)];
	}

	memcpy((void*)raw1D.data(), data, width * height * 4);

	if (rev)
	{
		for (int32_t i = 0; i < height / 2; i++)
		{
			png_bytep swp = raw2D[i];
			raw2D[i] = raw2D[height - i - 1];
			raw2D[height - i - 1] = swp;
		}
	}

	png_write_info(pp, ip);
	png_write_image(pp, raw2D.data());
	png_write_end(pp, ip);

	png_destroy_write_struct(&pp, &ip);
	fclose(fp);

	return true;
}

PNGHelper::PNGHelper()
{
}

PNGHelper::~PNGHelper()
{
}

bool PNGHelper::Save(const char16_t* path, int32_t width, int32_t height, const void* data)
{
	return SavePNGImage(path, width, height, data, false);
}

} // namespace efk
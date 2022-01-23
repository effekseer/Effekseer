#include "GifHelper.h"
#include <math.h>

namespace efk
{
GifHelper::~GifHelper()
{
	gdImageGifAnimEnd(fp);
	fclose(fp);
	gdImageDestroy(img);
}

bool GifHelper::Initialize(const char16_t* path, int32_t width, int32_t height, int32_t freq)
{
	img = gdImageCreate(width, height);

#ifdef _WIN32
	_wfopen_s(&fp, (const wchar_t*)path, L"wb");
#else
	char path8[1024];
	Effekseer::ConvertUtf16ToUtf8(path8, sizeof(path8), path);
	fp = fopen(path8, "wb");
#endif

	gdImageGifAnimBegin(img, fp, false, 0);

	this->width = width;
	this->height = height;
	this->freq = freq;

	return true;
}

void GifHelper::AddImage(const std::vector<Effekseer::Color>& pixels)
{
	int delay = (int)round((1.0 / (double)60.0 * freq) * 100.0);
	gdImagePtr frameImage = gdImageCreateTrueColor(width, height);

	for (int32_t y = 0; y < height; y++)
	{
		for (int32_t x = 0; x < width; x++)
		{
			auto c = pixels[x + y * width];
			gdImageSetPixel(frameImage, x, y, gdTrueColor(c.R, c.G, c.B));
		}
	}
	gdImageTrueColorToPalette(frameImage, true, gdMaxColors);
	gdImageGifAnimAdd(frameImage, fp, true, 0, 0, delay, gdDisposalNone, nullptr);
	gdImageDestroy(frameImage);
}
} // namespace efk
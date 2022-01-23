#include "AVIExporter.h"

namespace efk
{
typedef uint32_t FourCC;

const uint32_t AVIIF_Keyframe = 0x00000010L;

struct MainAVIHeader
{
	uint32_t dwMicroSecPerFrame;
	uint32_t dwMaxBytesPerSec;
	uint32_t dwPaddingGranularity;
	uint32_t dwFlags;
	uint32_t dwTotalFrames;
	uint32_t dwInitialFrames;
	uint32_t dwStreams;
	uint32_t dwSuggestedBufferSize;
	uint32_t dwWidth;
	uint32_t dwHeight;
	uint32_t dwReserved[4];
};

struct Rect
{
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;
};

struct AVIStreamHeader
{
	FourCC fccType;
	FourCC fccHandler;
	uint32_t dwFlags;
	uint16_t wPriority;
	uint16_t wLanguage;
	uint32_t dwInitialFrames;
	uint32_t dwScale;
	uint32_t dwRate;
	uint32_t dwStart;
	uint32_t dwLength;
	uint32_t dwSuggestedBufferSize;
	uint32_t dwQuality;
	uint32_t dwSampleSize;
	Rect rcFrame;
};

struct BitmapInfoHeader
{
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	int16_t biPlanes;
	int16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
};

struct AVIIndexEntry
{
	uint32_t ckid;
	uint32_t dwFlags;
	uint32_t dwChunkOffset;
	uint32_t dwChunkLength;
};

static FourCC GenFourCC(char ch0, char ch1, char ch2, char ch3)
{
	return ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) | ((uint32_t)(uint8_t)(ch2) << 16) |
			((uint32_t)(uint8_t)(ch3) << 24));
}

void AVIExporter::Initialize(int32_t width, int32_t height, int32_t framerate, int32_t frameCount)
{
	this->width = width;
	this->height = height;
	this->framerate = framerate;
	this->frameCount = frameCount;
}

void AVIExporter::BeginToExportAVI(std::vector<uint8_t>& dst)
{
	dst.clear();

	ChunkList aviChunkList;

	ChunkList hdriChunkList;

	Chunk avihChunk;
	MainAVIHeader mainAVIHeader;

	ChunkList strlChunkList;
	Chunk strhChunk;
	AVIStreamHeader aviStreamHeader;
	Chunk strfChunk;
	BitmapInfoHeader infoHeader;
	Chunk junkChunk;

	std::vector<uint8_t> junk;

	ChunkList moviChunkList;

	Chunk idx1Chunk;

	aviChunkList.Name = GenFourCC('R', 'I', 'F', 'F');
	aviChunkList.Size =
		2048 - 8 + (sizeof(Chunk) + width * height * 4) * frameCount + sizeof(idx1Chunk) + sizeof(AVIIndexEntry) * frameCount;
	aviChunkList.ID = GenFourCC('A', 'V', 'I', ' ');
	ExportData(dst, &aviChunkList);

	hdriChunkList.Name = GenFourCC('L', 'I', 'S', 'T');
	hdriChunkList.Size = sizeof(hdriChunkList) - 8 + sizeof(avihChunk) + sizeof(mainAVIHeader) + sizeof(strlChunkList) + sizeof(strhChunk) +
						 sizeof(aviStreamHeader) + sizeof(strfChunk) + sizeof(infoHeader);
	hdriChunkList.ID = GenFourCC('h', 'd', 'r', 'l');
	ExportData(dst, &hdriChunkList);

	avihChunk = Chunk(GenFourCC('a', 'v', 'i', 'h'), sizeof(mainAVIHeader));
	ExportData(dst, &avihChunk);

	mainAVIHeader.dwMicroSecPerFrame = 1000000 / (float)framerate;
	mainAVIHeader.dwMaxBytesPerSec = width * height * 4 * framerate;
	mainAVIHeader.dwPaddingGranularity = 0;
	mainAVIHeader.dwFlags = 2064;
	mainAVIHeader.dwTotalFrames = frameCount;
	mainAVIHeader.dwInitialFrames = 0;
	mainAVIHeader.dwStreams = 1;
	mainAVIHeader.dwSuggestedBufferSize = width * height * 4;
	mainAVIHeader.dwWidth = width;
	mainAVIHeader.dwHeight = height;
	ExportData(dst, &mainAVIHeader);

	strlChunkList.Name = GenFourCC('L', 'I', 'S', 'T');
	strlChunkList.Size = sizeof(strlChunkList) - 8 + sizeof(strhChunk) + sizeof(aviStreamHeader) + sizeof(strfChunk) + sizeof(infoHeader);
	strlChunkList.ID = GenFourCC('s', 't', 'r', 'l');
	ExportData(dst, &strlChunkList);

	strhChunk = Chunk(GenFourCC('s', 't', 'r', 'h'), sizeof(aviStreamHeader));
	ExportData(dst, &strhChunk);

	aviStreamHeader.fccType = GenFourCC('v', 'i', 'd', 's');
	aviStreamHeader.fccHandler = GenFourCC('D', 'I', 'B', ' ');
	aviStreamHeader.dwFlags = 0;
	aviStreamHeader.wPriority = 0;
	aviStreamHeader.wLanguage = 0;
	aviStreamHeader.dwInitialFrames = 0;
	aviStreamHeader.dwScale = 1;
	aviStreamHeader.dwRate = framerate;
	aviStreamHeader.dwStart = 0;
	aviStreamHeader.dwLength = frameCount;
	aviStreamHeader.dwSuggestedBufferSize = width * height * 4;
	aviStreamHeader.dwQuality = -1;
	aviStreamHeader.dwSampleSize = width * height * 4;
	ExportData(dst, &aviStreamHeader);

	strfChunk = Chunk(GenFourCC('s', 't', 'r', 'f'), sizeof(infoHeader));
	ExportData(dst, &strfChunk);

	infoHeader.biSize = sizeof(infoHeader);
	infoHeader.biWidth = width;
	infoHeader.biHeight = height;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 32;
	infoHeader.biCompression = 0;
	infoHeader.biSizeImage = width * height * 4;
	infoHeader.biXPelsPerMeter = 3780;
	infoHeader.biYPelsPerMeter = 3780;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;
	ExportData(dst, &infoHeader);

	junk.resize(2048 - 240);
	junkChunk = Chunk(GenFourCC('J', 'U', 'N', 'K'), junk.size());
	ExportData(dst, &junkChunk);
	ExportData(dst, junk);

	moviChunkList.Name = GenFourCC('L', 'I', 'S', 'T');
	moviChunkList.Size = sizeof(moviChunkList) - 8 + (sizeof(Chunk) + width * height * 4) * frameCount;
	moviChunkList.ID = GenFourCC('m', 'o', 'v', 'i');
	ExportData(dst, &moviChunkList);
}

void AVIExporter::ExportFrame(std::vector<uint8_t>& dst, std::vector<Effekseer::Color> frame)
{
	dst.clear();

	Chunk chunk;
	chunk.ID = GenFourCC('0', '0', 'd', 'b');
	chunk.Size = width * height * 4;
	ExportData(dst, &chunk);

	for (auto h = 0; h < height; h++)
	{
		for (auto w = 0; w < width; w++)
		{
			auto c = frame[w + (height - h - 1) * width];
			dst.push_back(c.B);
			dst.push_back(c.G);
			dst.push_back(c.R);
			dst.push_back(c.A);
		}
	}
}

void AVIExporter::FinishToExportAVI(std::vector<uint8_t>& dst)
{
	dst.clear();

	Chunk idx1Chunk;

	idx1Chunk = Chunk(GenFourCC('i', 'd', 'x', '1'), sizeof(AVIIndexEntry) * frameCount);
	ExportData(dst, &idx1Chunk);

	AVIIndexEntry entry;
	entry.ckid = GenFourCC('0', '0', 'd', 'b');
	entry.dwFlags = AVIIF_Keyframe;
	entry.dwChunkLength = width * height * 4;
	for (size_t i = 0; i < frameCount; i++)
	{
		entry.dwChunkOffset = 4 + (uint32_t)sizeof(Chunk) * i + width * height * 4 * i;
		ExportData(dst, &entry);
	}
}
} // namespace efk
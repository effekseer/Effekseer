
#pragma once

#include <Effekseer.h>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <vfw.h>
#endif

namespace efk
{
	class AVIExporter
	{
	private:
		int32_t	width;
		int32_t	height;
		int32_t framerate = 60;
		int32_t frameCount = 0;

		struct ChunkList
		{
			uint32_t Name;
			uint32_t Size;
			uint32_t ID;
		};

		struct Chunk
		{
			uint32_t ID;
			uint32_t Size;

			Chunk() {}
			Chunk(uint32_t id, uint32_t size)
			{
				ID = id;
				Size = size;
			}
		};

		template<typename T> void ExportData(std::vector<uint8_t>& dst, T* data)
		{
			auto u = (uint8_t*)data;
			for (size_t i = 0; i < sizeof(T); i++)
			{
				dst.push_back(u[i]);
			}
		}

		void ExportData(std::vector<uint8_t>& dst, std::vector<uint8_t>& data)
		{
			auto u = (uint8_t*)data.data();
			for (size_t i = 0; i < data.size(); i++)
			{
				dst.push_back(u[i]);
			}
		}

		void ExportData(std::vector<uint8_t>& dst, std::vector<Effekseer::Color>& data)
		{
			auto u = (uint8_t*)data.data();
			for (size_t i = 0; i < data.size() * 4; i++)
			{
				dst.push_back(u[i]);
			}
		}


	public:

		void Initialize(int32_t width, int32_t height, int32_t framerate, int32_t frameCount)
		{
			this->width = width;
			this->height = height;
			this->framerate = framerate;
			this->frameCount = frameCount;
		}

		void BeginToExportAVI(std::vector<uint8_t>& dst)
		{
#ifdef _WIN32
			dst.clear();

			ChunkList aviChunkList;

			ChunkList hdriChunkList;

			Chunk avihChunk;
			MainAVIHeader mainAVIHeader;

			ChunkList strlChunkList;
			Chunk  strhChunk;
			AVIStreamHeader aviStreamHeader;
			Chunk strfChunk;
			BITMAPINFOHEADER infoHeader;
			Chunk junkChunk;

			std::vector<uint8_t> junk;

			ChunkList moviChunkList;

			Chunk idx1Chunk;

			aviChunkList.Name = mmioFOURCC('R', 'I', 'F', 'F');
			aviChunkList.Size = 2048 - 8 + (sizeof(Chunk) + width * height * 4) * frameCount + sizeof(idx1Chunk) + sizeof(AVIINDEXENTRY) * frameCount;
			aviChunkList.ID = mmioFOURCC('A', 'V', 'I', ' ');
			ExportData(dst, &aviChunkList);

			hdriChunkList.Name = mmioFOURCC('L', 'I', 'S', 'T');
			hdriChunkList.Size =
				sizeof(hdriChunkList) - 8 +
				sizeof(avihChunk) + sizeof(mainAVIHeader) +
				sizeof(strlChunkList) +
				sizeof(strhChunk) + sizeof(aviStreamHeader) +
				sizeof(strfChunk) + sizeof(infoHeader);
			hdriChunkList.ID = mmioFOURCC('h', 'd', 'r', 'l');
			ExportData(dst, &hdriChunkList);

			avihChunk = Chunk(mmioFOURCC('a', 'v', 'i', 'h'), sizeof(mainAVIHeader));
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

			strlChunkList.Name = mmioFOURCC('L', 'I', 'S', 'T');
			strlChunkList.Size = sizeof(strlChunkList) - 8 + sizeof(strhChunk) + sizeof(aviStreamHeader) + sizeof(strfChunk) + sizeof(infoHeader);
			strlChunkList.ID = mmioFOURCC('s', 't', 'r', 'l');
			ExportData(dst, &strlChunkList);

			strhChunk = Chunk(mmioFOURCC('s', 't', 'r', 'h'), sizeof(aviStreamHeader));
			ExportData(dst, &strhChunk);

			aviStreamHeader.fccType = mmioFOURCC('v', 'i', 'd', 's');
			aviStreamHeader.fccHandler = mmioFOURCC('D', 'I', 'B', ' ');
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

			strfChunk = Chunk(mmioFOURCC('s', 't', 'r', 'f'), sizeof(infoHeader));
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
			junkChunk = Chunk(mmioFOURCC('J', 'U', 'N', 'K'), junk.size());
			ExportData(dst, &junkChunk);
			ExportData(dst, junk);

			moviChunkList.Name = mmioFOURCC('L', 'I', 'S', 'T');
			moviChunkList.Size = sizeof(moviChunkList) - 8 + (sizeof(Chunk) + width * height * 4) * frameCount;
			moviChunkList.ID = mmioFOURCC('m', 'o', 'v', 'i');
			ExportData(dst, &moviChunkList);
#endif
		}

		void ExportFrame(std::vector<uint8_t>& dst, std::vector<Effekseer::Color> frame)
		{
#ifdef _WIN32
			dst.clear();

			Chunk chunk;
			chunk.ID = mmioFOURCC('0', '0', 'd', 'b');
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
#endif
		}

		void FinishToExportAVI(std::vector<uint8_t>& dst)
		{
#ifdef _WIN32
			dst.clear();

			Chunk idx1Chunk;

			idx1Chunk = Chunk(mmioFOURCC('i', 'd', 'x', '1'), sizeof(AVIINDEXENTRY) * frameCount);
			ExportData(dst, &idx1Chunk);

			AVIINDEXENTRY entry;
			entry.ckid = mmioFOURCC('0', '0', 'd', 'b');
			entry.dwFlags = AVIIF_KEYFRAME;
			entry.dwChunkLength = width * height * 4;
			for (size_t i = 0; i < frameCount; i++)
			{
				entry.dwChunkOffset = 4 + sizeof(Chunk) * i + width * height * 4 * i;
				ExportData(dst, &entry);
			}
#endif
		}
	};

}

#pragma once

#include <Effekseer.h>
#include <vector>

namespace efk
{
class AVIExporter
{
private:
	int32_t width;
	int32_t height;
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

		Chunk()
		{
		}
		Chunk(uint32_t id, uint32_t size)
		{
			ID = id;
			Size = size;
		}
	};

	template <typename T>
	void ExportData(std::vector<uint8_t>& dst, T* data)
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
	void Initialize(int32_t width, int32_t height, int32_t framerate, int32_t frameCount);

	void BeginToExportAVI(std::vector<uint8_t>& dst);

	void ExportFrame(std::vector<uint8_t>& dst, std::vector<Effekseer::Color> frame);

	void FinishToExportAVI(std::vector<uint8_t>& dst);
};

} // namespace efk
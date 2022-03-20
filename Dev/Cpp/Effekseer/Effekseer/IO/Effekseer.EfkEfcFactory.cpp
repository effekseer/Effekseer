#include "Effekseer.EfkEfcFactory.h"
#include "../Utils/Effekseer.BinaryReader.h"

namespace Effekseer
{

EfkEfcFile::EfkEfcFile(const void* data, int32_t size)
	: data_(data)
	, size_(size)
{
	BinaryReader<true> binaryReader(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(data_)), size_);

	// EFKP
	int head = 0;
	if (!binaryReader.Read(head) || memcmp(&head, "EFKE", 4) != 0)
		return;

	if (!binaryReader.Read(version_))
		return;

	isValid_ = true;
}

EfkEfcFile::Chunk EfkEfcFile::ReadChunk(const char* forcc) const
{
	if (!IsValid())
		return {};

	BinaryReader<true> binaryReader(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(data_)), size_);

	// Skip forcc and version
	binaryReader.AddOffset(8);

	// read chunk
	while (binaryReader.GetOffset() < size_)
	{
		int chunkForcc = 0;
		binaryReader.Read(chunkForcc);
		int chunkSize = 0;
		binaryReader.Read(chunkSize);

		if (memcmp(&chunkForcc, forcc, 4) == 0)
		{
			Chunk chunk;
			chunk.data = reinterpret_cast<const uint8_t*>(data_) + binaryReader.GetOffset();
			chunk.size = chunkSize;
			return chunk;
		}

		binaryReader.AddOffset(chunkSize);
	}

	return {};
}

EfkEfcFile::Chunk EfkEfcFile::ReadInfo() const
{
	return ReadChunk("INFO");
}

EfkEfcFile::Chunk EfkEfcFile::ReadEditerData() const
{
	return ReadChunk("EDIT");
}

EfkEfcFile::Chunk EfkEfcFile::ReadRuntimeData() const
{
	return ReadChunk("BIN_");
}

bool EfkEfcFactory::OnLoading(Effect* effect, const void* data, int32_t size, float magnification, const char16_t* materialPath)
{
	EfkEfcFile file(data, size);

	if (!file.IsValid())
	{
		return false;
	}

	auto chunk = file.ReadRuntimeData();
	if (chunk.data == nullptr)
	{
		return false;
	}

	return LoadBody(effect, chunk.data, chunk.size, magnification, materialPath);
}

bool EfkEfcFactory::OnCheckIsBinarySupported(const void* data, int32_t size)
{
	EfkEfcFile file(data, size);

	return file.IsValid();
}

bool EfkEfcProperty::Load(const void* data, int32_t size)
{
	EfkEfcFile file(data, size);

	if (!file.IsValid())
	{
		return false;
	}

	auto chunk = file.ReadRuntimeData();
	if (chunk.data == nullptr)
	{
		return false;
	}

	BinaryReader<true> binaryReader(const_cast<uint8_t*>(static_cast<const uint8_t*>(chunk.data)), chunk.size);

	int32_t infoVersion = 0;

	auto loadStr = [this, &binaryReader, &infoVersion](std::vector<std::u16string>& dst) {
		int32_t dataCount = 0;
		binaryReader.Read(dataCount);

		// compatibility
		if (dataCount >= 1500)
		{
			infoVersion = dataCount;
			binaryReader.Read(dataCount);
		}

		dst.resize(dataCount);

		std::vector<char16_t> strBuf;

		for (int i = 0; i < dataCount; i++)
		{
			int length = 0;
			binaryReader.Read(length);
			strBuf.resize(length);
			binaryReader.Read(strBuf.data(), length);
			dst.at(i) = strBuf.data();
		}
	};

	loadStr(colorImages_);
	loadStr(normalImages_);
	loadStr(distortionImages_);
	loadStr(models_);
	loadStr(sounds_);

	if (infoVersion >= 1500)
	{
		loadStr(materials_);
	}

	return true;
}

const std::vector<std::u16string>& EfkEfcProperty::GetColorImages() const
{
	return colorImages_;
}
const std::vector<std::u16string>& EfkEfcProperty::GetNormalImages() const
{
	return normalImages_;
}
const std::vector<std::u16string>& EfkEfcProperty::GetDistortionImages() const
{
	return distortionImages_;
}
const std::vector<std::u16string>& EfkEfcProperty::GetSounds() const
{
	return sounds_;
}
const std::vector<std::u16string>& EfkEfcProperty::GetModels() const
{
	return models_;
}
const std::vector<std::u16string>& EfkEfcProperty::GetMaterials() const
{
	return materials_;
}

} // namespace Effekseer
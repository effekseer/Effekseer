#include "FileInterface.h"
#include <IO/IO.h>

namespace Effekseer::Tool
{

namespace
{

class EffekseerFileReader : public Effekseer::FileReader
{
	std::vector<uint8_t> data;
	int32_t position;

public:
	EffekseerFileReader(std::vector<uint8_t>& data)
	{
		this->data = data;
		position = 0;
	}

	virtual ~EffekseerFileReader()
	{
	}

	size_t Read(void* buffer, size_t size)
	{
		auto readable = size;
		if (data.size() - position < size)
			readable = data.size() - position;

		memcpy(buffer, &(data[position]), readable);
		position += static_cast<int>(readable);
		return readable;
	}

	void Seek(int position)
	{
		this->position = position;
		if (this->position < 0)
			this->position = 0;
		if (this->position > static_cast<int>(data.size()))
			this->position = static_cast<int>(data.size());
	}

	int GetPosition() const
	{
		return position;
	}

	size_t GetLength() const
	{
		return data.size();
	}
};

} // namespace

Effekseer::FileReaderRef EffekseerFile::OpenRead(const EFK_CHAR* path)
{
	std::shared_ptr<Effekseer::StaticFile> staticFile;

	if (staticFile == nullptr)
	{
		staticFile = ::Effekseer::IO::GetInstance()->LoadIPCFile(path);
	}

	if (staticFile == nullptr)
	{
		staticFile = ::Effekseer::IO::GetInstance()->LoadFile(path);
	}

	if (staticFile != nullptr)
	{
		std::vector<uint8_t> data;

		data.resize(staticFile->GetSize());
		memcpy(data.data(), staticFile->GetData(), staticFile->GetSize());
		return Effekseer::MakeRefPtr<EffekseerFileReader>(data);
	}

	return nullptr;
}

Effekseer::FileWriterRef EffekseerFile::OpenWrite(const EFK_CHAR* path)
{
	return nullptr;
}

} // namespace Effekseer::Tool
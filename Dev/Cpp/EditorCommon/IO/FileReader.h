#pragma once

#include <string>
#include <vector>

namespace Effekseer
{
enum class StaticFileType
{
	Default,
	IPC,
};

#ifndef SWIG

class StaticFileReader
{
protected:
	std::u16string path_;
	int64_t length_ = -1;

public:
	StaticFileReader(const std::u16string& path)
		: path_(path)
	{
	}
	virtual ~StaticFileReader() = default;

	const std::u16string& GetPath()
	{
		return path_;
	}

	virtual int64_t GetSize() = 0;
	virtual void GetData(std::vector<uint8_t>& buffer) = 0;

	virtual StaticFileType GetFileType() = 0;
};

#endif

} // namespace Effekseer
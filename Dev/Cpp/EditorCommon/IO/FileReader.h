#pragma once

#include <string>
#include <vector>

namespace Effekseer
{
enum class IOFileType
{
	Default,
	IPC,
};

#ifndef SWIG

class IOFileReader
{
protected:
	std::u16string path_;
	int64_t length_ = 0;

public:
	IOFileReader(const std::u16string& path) : path_(path) {}
	virtual ~IOFileReader() = default;

	const std::u16string& GetPath() { return path_; }

	virtual int64_t GetSize() = 0;
	virtual void GetData(std::vector<uint8_t>& buffer) = 0;

	virtual IOFileType GetFileType() = 0;
};

#endif

} // namespace Effekseer
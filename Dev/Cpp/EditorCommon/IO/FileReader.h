#pragma once

#include <string>
#include <vector>

namespace Effekseer
{
enum class FileType
{
	Default,
	IPC,
};

class FileReader
{
protected:
	std::u16string path_;
	int64_t length_;

public:
	FileReader(const std::u16string& path) : path_(path) {}
	virtual ~FileReader();

	const std::u16string& GetPath() { return path_; }

	virtual int64_t GetSize() = 0;
	virtual void GetData(std::vector<uint8_t>& buffer) = 0;

	virtual FileType GetFileType() = 0;
};

} // namespace Effekseer
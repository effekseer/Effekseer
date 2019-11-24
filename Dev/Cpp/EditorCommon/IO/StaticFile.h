#pragma once

#include "FileReader.h"
#include <memory>
#include <vector>

namespace Effekseer
{

class StaticFile
{
private:
	std::shared_ptr<FileReader> reader_;
	std::vector<uint8_t> buffer_;

public:
	StaticFile(std::shared_ptr<FileReader>& reader);
	~StaticFile();

	void* GetData();
	int GetSize();
	std::u16string GetPath();
	FileType GetFileType();
};

} // namespace Effekseer
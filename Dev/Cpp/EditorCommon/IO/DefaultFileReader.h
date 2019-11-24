#pragma once

#include "FileReader.h"
#include <fstream>

namespace Effekseer
{

class DefaultFileReader : public FileReader
{
private:
	std::ifstream stream_;

public:
	DefaultFileReader(const std::u16string& path);
	virtual ~DefaultFileReader();

	int64_t GetSize() override;
	void GetData(std::vector<uint8_t>& buffer) override;

	FileType GetFileType() override { return FileType::Default; }
};

} // namespace Effekseer
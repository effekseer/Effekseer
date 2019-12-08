#pragma once

#include "FileReader.h"
#include <fstream>

namespace Effekseer
{

class IODefaultFileReader : public IOFileReader
{
private:
	std::ifstream stream_;

public:
	IODefaultFileReader(const std::u16string& path);
	virtual ~IODefaultFileReader();

	int64_t GetSize() override;
	void GetData(std::vector<uint8_t>& buffer) override;

	IOFileType GetFileType() override { return IOFileType::Default; }
};

} // namespace Effekseer
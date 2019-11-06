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

	void ReadBytes(std::vector<uint8_t>& buffer, const int64_t size = 0) override;
	
	void Seek(const int64_t offset, const SeekOrigin origin = SeekOrigin::Begin) override;
};

} // namespace Effekseer
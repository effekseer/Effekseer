#pragma once

#include "FileReader.h"
#include <fstream>

namespace Effekseer
{

class DefaultStaticFileReader : public StaticFileReader
{
private:
	std::u16string path_;
	std::ifstream stream_;
	std::vector<uint8_t> buffer_;

public:
	DefaultStaticFileReader(const std::u16string& path);
	virtual ~DefaultStaticFileReader();

	int64_t GetSize() override;
	void GetData(std::vector<uint8_t>& buffer) override;

	StaticFileType GetFileType() override
	{
		return StaticFileType::Default;
	}
};

} // namespace Effekseer
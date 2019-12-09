#include "DefaultFileReader.h"
#include "../Common/StringHelper.h"
#include <cassert>

namespace Effekseer
{

DefaultStaticFileReader::DefaultStaticFileReader(const std::u16string& path) : StaticFileReader(path)
{
#ifdef _WIN32
	stream_.open((wchar_t*)path.c_str(), std::basic_ios<char>::in | std::basic_ios<char>::binary);
#else
	stream_.open(utf16_to_utf8(path).c_str(), std::basic_ios<char>::in | std::basic_ios<char>::binary);
#endif
	assert(!stream_.fail());
}

DefaultStaticFileReader::~DefaultStaticFileReader() { stream_.close(); }

int64_t DefaultStaticFileReader::GetSize()
{
	if (length_ < 0)
	{
		assert(!stream_.fail());
		stream_.seekg(0, std::ios_base::end);
		length_ = stream_.tellg();
		stream_.clear();
		stream_.seekg(0, std::ios_base::beg);
	}
	return length_;
}

void DefaultStaticFileReader::GetData(std::vector<uint8_t>& buffer)
{
	buffer.resize(GetSize());
	stream_.read(reinterpret_cast<char*>(&buffer[0]), GetSize());
}

} // namespace Effekseer
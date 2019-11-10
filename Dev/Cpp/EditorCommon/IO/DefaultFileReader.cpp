#include "DefaultFileReader.h"
#include <cassert>

namespace Effekseer
{
DefaultFileReader::DefaultFileReader(const std::u16string& path) : FileReader(path)
{
#ifdef _WIN32
	stream_.open((wchar_t*)path.c_str(), std::basic_ios<char>::in | std::basic_ios<char>::binary);
#else
	stream_.open(utf16_to_utf8(path).c_str(), std::basic_ios<char>::in | std::basic_ios<char>::binary);
#endif
	assert(!stream_.fail());
}

DefaultFileReader::~DefaultFileReader() { stream_.close(); }

int64_t DefaultFileReader::GetSize()
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

void DefaultFileReader::GetData(std::vector<uint8_t>& buffer)
{
	buffer.resize(GetSize());
	stream_.read(reinterpret_cast<char*>(&buffer[0]), GetSize());
}

} // namespace Effekseer
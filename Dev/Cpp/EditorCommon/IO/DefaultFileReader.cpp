#include "DefaultFileReader.h"
#include "../Common/StringHelper.h"
#include <cassert>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

namespace Effekseer
{

DefaultStaticFileReader::DefaultStaticFileReader(const std::u16string& path)
	: StaticFileReader(path)
	, path_(path)
{
#ifdef _WIN32
	stream_.open((wchar_t*)path.c_str(), std::basic_ios<char>::in | std::basic_ios<char>::binary);
#else
	stream_.open(Effekseer::Tool::StringHelper::ConvertUtf16ToUtf8(path).c_str(), std::basic_ios<char>::in | std::basic_ios<char>::binary);
#endif

	if (stream_.fail())
	{
		spdlog::trace("DefaultStaticFileReader : {} : Failed to load.", Tool::StringHelper::ConvertUtf16ToUtf8(path));
		length_ = 0;
		return;
	}
	else
	{
		spdlog::trace("DefaultStaticFileReader : {} : Succseeded in loading.", Tool::StringHelper::ConvertUtf16ToUtf8(path));
	}

	if (length_ < 0)
	{
		assert(!stream_.fail());
		stream_.seekg(0, std::ios_base::end);
		length_ = stream_.tellg();
		stream_.clear();
		stream_.seekg(0, std::ios_base::beg);
	}

	buffer_.resize(GetSize());

	if (buffer_.size() > 0)
	{
		stream_.read(reinterpret_cast<char*>(buffer_.data()), GetSize());
	}
	else
	{
		spdlog::trace("DefaultStaticFileReader : {} : Size is zero.", Tool::StringHelper::ConvertUtf16ToUtf8(path));
	}
	stream_.close();
}

DefaultStaticFileReader::~DefaultStaticFileReader()
{
}

int64_t DefaultStaticFileReader::GetSize()
{
	return length_;
}

void DefaultStaticFileReader::GetData(std::vector<uint8_t>& buffer)
{
	buffer = buffer_;
}

} // namespace Effekseer
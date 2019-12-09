#include "IPCFileReader.h"
#include "../Common/StringHelper.h"

namespace Effekseer
{

const int MAX_STORAGE = 104857600;

IPCFileReader::IPCFileReader(const std::u16string& path, std::shared_ptr<IPC::KeyValueFileStorage> storage) : StaticFileReader(path)
{
	storage->Lock();

	buffer_.resize(MAX_STORAGE);
	length_ = storage->GetFile(utf16_to_utf8(path).c_str(), buffer_.data(), MAX_STORAGE, time_);
	buffer_.resize(length_);

	storage->Unlock();
}

IPCFileReader::~IPCFileReader() {}

int64_t IPCFileReader::GetSize() { return length_; }

void IPCFileReader::GetData(std::vector<uint8_t>& buffer)
{
	buffer.resize(GetSize());
	std::copy(buffer_.begin(), buffer_.end(), buffer.begin());
}

} // namespace Effekseer
#include "IPCFileReader.h"
#include "../Common/StringHelper.h"

namespace Effekseer
{

const int MAX_STORAGE = 104857600;

IPCFileReader::IPCFileReader(const std::u16string& path, std::shared_ptr<IPC::KeyValueFileStorage> storage) : StaticFileReader(path)
{
	path_ = utf16_to_utf8(path);

	storage->Lock();

	buffer_.resize(MAX_STORAGE);
	length_ = storage->GetFile(path_.c_str(), buffer_.data(), MAX_STORAGE, time_);
	buffer_.resize(length_);

	if (length_ > 0)
	{
		hasRef_ = true;
		storage->AddRef(path_.c_str());
	}

	storage->Unlock();

	storage_ = storage;
}

IPCFileReader::~IPCFileReader()
{
	if (hasRef_)
	{
		storage_->Lock();
		storage_->ReleaseRef(path_.c_str());
		storage_->Unlock();
	}
}

int64_t IPCFileReader::GetSize() { return length_; }

void IPCFileReader::GetData(std::vector<uint8_t>& buffer)
{
	buffer.resize(GetSize());
	std::copy(buffer_.begin(), buffer_.end(), buffer.begin());
}

} // namespace Effekseer
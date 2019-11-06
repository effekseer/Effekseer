#pragma once

#include "FileReader.h"
#include "../IPC/IPC.h"

namespace Effekseer
{

class IPCFileReader : public FileReader
{
private:
	IPC::KeyValueFileStorage storage_;

public:
	IPCFileReader(const std::u16string& path);
	virtual ~IPCFileReader();

	int64_t GetSize() override;

	void ReadBytes(std::vector<uint8_t>& buffer, const int64_t size = 0) override;

	void Seek(const int64_t offset, const SeekOrigin origin = SeekOrigin::Begin) override;
};

} // namespace Effekseer
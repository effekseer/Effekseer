#pragma once

#include "FileReader.h"
#include "../IPC/IPC.h"

namespace Effekseer
{

class IPCFileReader : public FileReader
{
private:
	IPC::KeyValueFileStorage* storage_;

public:
	IPCFileReader(const std::u16string& path);
	virtual ~IPCFileReader();

	int64_t GetSize() override;
	void GetData(std::vector<uint8_t>& buffer) override;
};

} // namespace Effekseer
#pragma once

#include "../IPC/IPC.h"
#include "FileReader.h"
#include <memory>

namespace Effekseer
{

class IPCFileReader : public FileReader
{
private:
	std::vector<uint8_t> buffer_;
	int time_;

public:
	IPCFileReader(const std::u16string& path, std::shared_ptr<IPC::KeyValueFileStorage> storage);
	virtual ~IPCFileReader();

	int64_t GetSize() override;
	void GetData(std::vector<uint8_t>& buffer) override;

	FileType GetFileType() override { return FileType::IPC; }

	int GetUpdateTime() { return time_; }
};

} // namespace Effekseer
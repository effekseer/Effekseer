#pragma once

#include "../IPC/IPC.h"
#include "FileReader.h"
#include <memory>

namespace Effekseer
{

class IPCFileReader : public StaticFileReader
{
private:
	std::vector<uint8_t> buffer_;
	int time_;

public:
	IPCFileReader(const std::u16string& path, std::shared_ptr<IPC::KeyValueFileStorage> storage);
	virtual ~IPCFileReader();

	int64_t GetSize() override;
	void GetData(std::vector<uint8_t>& buffer) override;

	StaticFileType GetFileType() override { return StaticFileType::IPC; }

	int GetUpdateTime() { return time_; }
};

} // namespace Effekseer
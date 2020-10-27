#pragma once

#include "../IPC/IPC.h"
#include "FileReader.h"
#include <memory>

namespace Effekseer
{

class IPCFileReader : public StaticFileReader
{
private:
	std::string path_;
	std::shared_ptr<IPC::KeyValueFileStorage> storage_;
	std::vector<uint8_t> buffer_;
	uint64_t time_ = 0;
	bool hasRef_ = false;

public:
	IPCFileReader(const std::u16string& path, std::shared_ptr<IPC::KeyValueFileStorage> storage);
	virtual ~IPCFileReader();

	int64_t GetSize() override;
	void GetData(std::vector<uint8_t>& buffer) override;

	StaticFileType GetFileType() override
	{
		return StaticFileType::IPC;
	}

	uint64_t GetUpdateTime()
	{
		return time_;
	}
};

} // namespace Effekseer
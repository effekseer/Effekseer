#pragma once

#include "../IPC/IPC.h"
#include "StaticFile.h"
#include <memory>

namespace Effekseer
{

class IO
{
private:
	std::shared_ptr<IPC::KeyValueFileStorage> ipcStorage_;

public:
	IO(bool isEnableAutoUpdate);
	~IO();

	std::shared_ptr<StaticFile> LoadFile(const char16_t* path);
	std::shared_ptr<StaticFile> LoadIPCFile(const char16_t* path);

	std::shared_ptr<IPC::KeyValueFileStorage> GetIPCStorage() { return ipcStorage_; }
};

} // namespace Effekseer
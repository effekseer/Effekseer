#pragma once

#include "../IPC/IPC.h"
#include <memory>

namespace Effekseer
{

class IO
{
private:
	std::shared_ptr<IPC::KeyValueFileStorage> ipcStorage_;

public:
	IO();
	~IO();

	std::shared_ptr<IPC::KeyValueFileStorage> GetIPCStorage() { return ipcStorage_; }
};

} // namespace Effekseer
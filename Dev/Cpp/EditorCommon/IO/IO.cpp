#include "IO.h"
#include "DefaultFileReader.h"
#include "IPCFileReader.h"
#include <filesystem>

namespace Effekseer
{
IO::IO(bool isEnableAutoUpdate)
{
	ipcStorage_ = std::make_shared<IPC::KeyValueFileStorage>();
	ipcStorage_->Start("EffekseerStorage");
}

IO::~IO() { ipcStorage_->Stop(); }

std::shared_ptr<StaticFile> IO::LoadFile(const char16_t* path)
{
	if (!std::filesystem::exists(path))
		return nullptr;

	auto reader = std::make_shared<DefaultFileReader>(path);
	return std::make_shared<StaticFile>(reader);
}

std::shared_ptr<StaticFile> IO::LoadIPCFile(const char16_t* path)
{
	if (!std::filesystem::exists(path))
		return nullptr;

	auto reader = std::make_shared<IPCFileReader>(path, GetIPCStorage());
	return std::make_shared<StaticFile>(reader);
}

} // namespace Effekseer
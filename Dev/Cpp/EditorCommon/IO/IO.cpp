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
	if (!std::filesystem::is_regular_file(path))
		return nullptr;

	std::shared_ptr<FileReader> reader = std::make_shared<DefaultFileReader>(path);
	auto file = std::make_shared<StaticFile>(reader);

	auto time = std::filesystem::last_write_time(path).time_since_epoch().count();
	fileUpdateDates_[FileInfo(file->GetFileType(), file->GetPath())] = time;

	return file;
}

std::shared_ptr<StaticFile> IO::LoadIPCFile(const char16_t* path)
{
	if (!std::filesystem::is_regular_file(path))
		return nullptr;

	std::shared_ptr<FileReader> reader = std::make_shared<IPCFileReader>(path, GetIPCStorage());
	auto file = std::make_shared<StaticFile>(reader);

	auto time = 0; // TODO:Get IPC file's last write time.
	fileUpdateDates_[FileInfo(file->GetFileType(), file->GetPath())] = time;

	return file;
}

bool IO::GetIsExistLatestFile(std::shared_ptr<StaticFile> staticFile)
{
	int time;
	switch (staticFile->GetFileType())
	{
	case FileType::Default:
		time = std::filesystem::last_write_time(staticFile->GetPath()).time_since_epoch().count();
		break;
	case FileType::IPC:
		// TODO:Get IPC file's last write time.
		break;
	default:
		return nullptr;
	}

	return time > fileUpdateDates_[FileInfo(staticFile->GetFileType(), staticFile->GetPath())];
}

} // namespace Effekseer
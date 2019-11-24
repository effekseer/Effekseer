#pragma once

#include "../IPC/IPC.h"
#include "StaticFile.h"
#include <filesystem>
#include <map>
#include <memory>

namespace Effekseer
{

class IO
{
private:
	std::shared_ptr<IPC::KeyValueFileStorage> ipcStorage_;

	struct FileInfo
	{
	private:
		FileType fileType_;
		std::u16string path_;

	public:
		FileInfo(FileType fileType, std::u16string path) : fileType_(fileType), path_(path) {}

		bool operator==(const FileInfo& right) { return fileType_ == right.fileType_ && path_ == right.path_; }
		bool operator<(const FileInfo& right) const { return std::tie(fileType_, path_) < std::tie(right.fileType_, right.path_); }
	};

	std::map<FileInfo, int> fileUpdateDates_;

public:
	IO(bool isEnableAutoUpdate);
	~IO();

	std::shared_ptr<StaticFile> LoadFile(const char16_t* path);
	std::shared_ptr<StaticFile> LoadIPCFile(const char16_t* path);

	bool GetIsExistLatestFile(std::shared_ptr<StaticFile> staticFile);

	std::shared_ptr<IPC::KeyValueFileStorage> GetIPCStorage() { return ipcStorage_; }
};

} // namespace Effekseer
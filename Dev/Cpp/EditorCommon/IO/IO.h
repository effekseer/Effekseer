#pragma once

#include "../IPC/IPC.h"
#include "StaticFile.h"
#include <map>
#include <memory>
#include <thread>
#include <set>
#include <mutex>

namespace Effekseer
{

class IO
{
private:
	std::shared_ptr<IPC::KeyValueFileStorage> ipcStorage_;

	struct FileInfo
	{
		FileType fileType_;
		std::u16string path_;

		FileInfo(FileType fileType, std::u16string path) : fileType_(fileType), path_(path) {}

		bool operator==(const FileInfo& right) { return fileType_ == right.fileType_ && path_ == right.path_; }
		bool operator<(const FileInfo& right) const { return std::tie(fileType_, path_) < std::tie(right.fileType_, right.path_); }
	};

	std::map<FileInfo, int> fileUpdateDates_;
	std::thread checkFileThread;
	bool isFinishCheckFile_ = false;
	
	std::set<FileInfo> changedFileInfos_;
	std::set<FileInfo> notifiedFileInfos_;
	std::mutex mtx_;

public:
	IO(int checkFileInterval = 0);
	~IO();

	std::shared_ptr<StaticFile> LoadFile(const char16_t* path);
	std::shared_ptr<StaticFile> LoadIPCFile(const char16_t* path);

	bool GetIsExistLatestFile(std::shared_ptr<StaticFile> staticFile);

	std::shared_ptr<IPC::KeyValueFileStorage> GetIPCStorage() { return ipcStorage_; }

	void Update();

	virtual void OnFileChanged(FileType fileType, std::u16string path) {}

private:
	int GetFileLastWriteTime(const FileInfo& fileInfo);
	void CheckFile(int interval);
};

} // namespace Effekseer
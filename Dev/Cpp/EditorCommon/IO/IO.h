#pragma once

#include "../IPC/IPC.h"
#include "StaticFile.h"
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <thread>

namespace Effekseer
{

class IOCallback
{
public:
	virtual void OnFileChanged(StaticFileType fileType, const char16_t* path)
	{
	}
};

class IO
{
private:
	std::u16string ReplaceSeparator(const char16_t* path);

	static std::shared_ptr<IO> instance_;

	std::shared_ptr<IPC::KeyValueFileStorage> ipcStorage_;
	std::vector<std::shared_ptr<IOCallback>> callbacks_;

	struct FileInfo
	{
		StaticFileType fileType_;
		std::u16string path_;

		FileInfo(StaticFileType fileType, std::u16string path)
			: fileType_(fileType)
			, path_(path)
		{
		}

		bool operator==(const FileInfo& right)
		{
			return fileType_ == right.fileType_ && path_ == right.path_;
		}
		bool operator<(const FileInfo& right) const
		{
			return std::tie(fileType_, path_) < std::tie(right.fileType_, right.path_);
		}
	};

	std::map<FileInfo, uint64_t> fileUpdateDates_;
	std::thread checkFileThread;
	bool isFinishCheckFile_ = false;
	bool isThreadRunning_ = false;
	int32_t checkFileInterval_ = -1;

	std::set<FileInfo> changedFileInfos_;
	std::mutex mtx_;

public:
	IO(int checkFileInterval = 0);
	virtual ~IO();

	static std::shared_ptr<IO> GetInstance();

	static void Initialize(int checkFileInterval = 0);

	static void Terminate();

	std::shared_ptr<StaticFile> LoadFile(const char16_t* path);
	std::shared_ptr<StaticFile> LoadIPCFile(const char16_t* path);

	bool GetIsExistLatestFile(std::shared_ptr<StaticFile> staticFile);

#ifndef SWIG
	std::shared_ptr<IPC::KeyValueFileStorage> GetIPCStorage()
	{
		return ipcStorage_;
	}
#endif

	void Update();

	void AddCallback(std::shared_ptr<IOCallback> callback);

private:
	uint64_t GetFileLastWriteTime(const FileInfo& fileInfo);
	void CheckFile(int interval);
};

} // namespace Effekseer
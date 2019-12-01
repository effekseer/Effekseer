#include "IO.h"
#include "../Common/StringHelper.h"
#include "../Platform/FileSystem.h"
#include "DefaultFileReader.h"
#include "IPCFileReader.h"

namespace Effekseer
{
IO::IO(int checkFileInterval)
{
	ipcStorage_ = std::make_shared<IPC::KeyValueFileStorage>();
	ipcStorage_->Start("EffekseerStorage");

	if (checkFileInterval > 0)
	{
		checkFileThread = std::thread([&] { CheckFile(checkFileInterval); });
	}
}

IO::~IO()
{
	ipcStorage_->Stop();
	if (checkFileThread.joinable())
		isFinishCheckFile_ = true;
	checkFileThread.join();
}

std::shared_ptr<StaticFile> IO::LoadFile(const char16_t* path)
{
	if (!FileSystem::GetIsFile(path))
		return nullptr;

	std::shared_ptr<FileReader> reader = std::make_shared<DefaultFileReader>(path);
	auto file = std::make_shared<StaticFile>(reader);

	auto info = FileInfo(file->GetFileType(), file->GetPath());
	{
		std::lock_guard<std::mutex> lock(mtx_);
		changedFileInfos_.erase(info);
		notifiedFileInfos_.erase(info);
	}

	auto time = FileSystem::GetLastWriteTime(path);
	fileUpdateDates_[info] = time;

	return file;
}

std::shared_ptr<StaticFile> IO::LoadIPCFile(const char16_t* path)
{
	if (!FileSystem::GetIsFile(path))
		return nullptr;

	std::shared_ptr<FileReader> reader = std::make_shared<IPCFileReader>(path, GetIPCStorage());
	auto file = std::make_shared<StaticFile>(reader);

	auto info = FileInfo(file->GetFileType(), file->GetPath());
	{
		std::lock_guard<std::mutex> lock(mtx_);
		changedFileInfos_.erase(info);
		notifiedFileInfos_.erase(info);
	}

	auto time = std::dynamic_pointer_cast<IPCFileReader>(reader)->GetUpdateTime();
	fileUpdateDates_[info] = time;

	return file;
}

bool IO::GetIsExistLatestFile(std::shared_ptr<StaticFile> staticFile)
{
	auto info = FileInfo(staticFile->GetFileType(), staticFile->GetPath());
	auto time = GetFileLastWriteTime(info);
	return time > fileUpdateDates_[info];
}

void IO::Update()
{
	std::lock_guard<std::mutex> lock(mtx_);
	for (auto& i : changedFileInfos_)
	{
		OnFileChanged(i.fileType_, i.path_);
		notifiedFileInfos_.insert(i);
	}

	changedFileInfos_.clear();
}

int IO::GetFileLastWriteTime(const FileInfo& fileInfo)
{
	int time = 0;
	switch (fileInfo.fileType_)
	{
	case FileType::Default:
		time = FileSystem::GetLastWriteTime(fileInfo.path_);
		break;
	case FileType::IPC:
		char data;
		if (ipcStorage_->GetFile(utf16_to_utf8(fileInfo.path_).c_str(), &data, 1, time) == 0)
			return 0;
		break;
	}
	return time;
}

void IO::CheckFile(int interval)
{
	while (!isFinishCheckFile_)
	{
		for (auto& i : fileUpdateDates_)
		{
			auto time = GetFileLastWriteTime(i.first);
			if (time > fileUpdateDates_[i.first] && changedFileInfos_.count(i.first) > 0 && notifiedFileInfos_.count(i.first) > 0)
			{
				std::lock_guard<std::mutex> lock(mtx_);
				changedFileInfos_.insert(i.first);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(interval));
	}
}

} // namespace Effekseer
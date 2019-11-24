#include "IO.h"
#include "DefaultFileReader.h"
#include "IPCFileReader.h"
#include <filesystem>

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
	if (!std::filesystem::is_regular_file(path))
		return nullptr;

	std::shared_ptr<FileReader> reader = std::make_shared<DefaultFileReader>(path);
	auto file = std::make_shared<StaticFile>(reader);

	auto info = FileInfo(file->GetFileType(), file->GetPath());
	{
		std::lock_guard<std::mutex> lock(mtx_);
		changedFileInfos_.erase(info);
		notifiedFileInfos_.erase(info);
	}

	auto time = std::filesystem::last_write_time(path).time_since_epoch().count();
	fileUpdateDates_[info] = time;

	return file;
}

std::shared_ptr<StaticFile> IO::LoadIPCFile(const char16_t* path)
{
	if (!std::filesystem::is_regular_file(path))
		return nullptr;

	std::shared_ptr<FileReader> reader = std::make_shared<IPCFileReader>(path, GetIPCStorage());
	auto file = std::make_shared<StaticFile>(reader);

	auto info = FileInfo(file->GetFileType(), file->GetPath());
	{
		std::lock_guard<std::mutex> lock(mtx_);
		changedFileInfos_.erase(info);
		notifiedFileInfos_.erase(info);
	}

	auto time = 0; // TODO:Get IPC file's last write time.
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
		time = std::filesystem::last_write_time(fileInfo.path_).time_since_epoch().count();
		break;
	case FileType::IPC:
		// TODO:Get IPC file's last write time.
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
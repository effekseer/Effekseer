#include "IO.h"
#include "../Common/StringHelper.h"
#include "../Platform/FileSystem.h"
#include "DefaultFileReader.h"
#include "IPCFileReader.h"

namespace Effekseer
{

std::shared_ptr<IO> IO::instance_;

std::shared_ptr<IO> IO::GetInstance()
{
	return instance_;
}

std::u16string IO::ReplaceSeparator(const char16_t* path)
{
#if _WIN32
	std::u16string pathSafe = path;

	for (size_t i = 0; i < pathSafe.size(); i++)
	{
		if (pathSafe[i] == u'\\')
		{
			pathSafe[i] = u'/';
		}
	}
	return pathSafe;
#endif

	return path;
}

void IO::Initialize(int checkFileInterval)
{
	instance_ = std::make_shared<IO>(checkFileInterval);
}

void IO::Terminate()
{
	instance_ = nullptr;
}

IO::IO(int checkFileInterval)
{
	ipcStorage_ = std::make_shared<IPC::KeyValueFileStorage>();
	ipcStorage_->Start("EfkStorage");

	this->checkFileInterval_ = checkFileInterval;

	if (checkFileInterval > 0)
	{
		checkFileThread = std::thread([this] { CheckFile(checkFileInterval_); });
		isThreadRunning_ = true;
	}
}

IO::~IO()
{
	{
		std::lock_guard<std::mutex> lock(mtx_);
		ipcStorage_->Stop();
		ipcStorage_.reset();
	}

	if (isThreadRunning_)
	{
		if (checkFileThread.joinable())
			isFinishCheckFile_ = true;
		checkFileThread.join();
	}
}

std::shared_ptr<StaticFile> IO::LoadFile(const char16_t* path)
{
	auto pathSafe = ReplaceSeparator(path);

	if (!FileSystem::GetIsFile(pathSafe))
		return nullptr;

	std::lock_guard<std::mutex> lock(mtx_);

	std::shared_ptr<StaticFileReader> reader = std::make_shared<DefaultStaticFileReader>(pathSafe);
	auto file = std::make_shared<StaticFile>(reader);

	auto info = FileInfo(file->GetFileType(), file->GetPath());
	{
		changedFileInfos_.erase(info);
	}

	auto time = FileSystem::GetLastWriteTime(pathSafe);
	fileUpdateDates_[info] = time;

	return file;
}

std::shared_ptr<StaticFile> IO::LoadIPCFile(const char16_t* path)
{
	auto pathSafe = ReplaceSeparator(path);

	if (!FileSystem::GetIsFile(pathSafe))
		return nullptr;

	std::lock_guard<std::mutex> lock(mtx_);

	std::shared_ptr<StaticFileReader> reader = std::make_shared<IPCFileReader>(pathSafe, GetIPCStorage());
	auto file = std::make_shared<StaticFile>(reader);

	if (file->GetSize() == 0)
	{
		return nullptr;
	}

	auto info = FileInfo(file->GetFileType(), file->GetPath());
	{
		changedFileInfos_.erase(info);
	}

	auto time = std::dynamic_pointer_cast<IPCFileReader>(reader)->GetUpdateTime();
	fileUpdateDates_[info] = time;

	return file;
}

bool IO::GetIsExistLatestFile(std::shared_ptr<StaticFile> staticFile)
{
	std::lock_guard<std::mutex> lock(mtx_);
	auto info = FileInfo(staticFile->GetFileType(), staticFile->GetPath());
	auto time = GetFileLastWriteTime(info);
	return time > fileUpdateDates_[info];
}

void IO::Update()
{
	std::set<FileInfo> temp;
	{
		std::lock_guard<std::mutex> lock(mtx_);
		temp = changedFileInfos_;
		changedFileInfos_.clear();
	}

	for (auto& i : temp)
	{
		for (auto callback : callbacks_)
		{
			callback->OnFileChanged(i.fileType_, i.path_.c_str());
		}
	}
}

uint64_t IO::GetFileLastWriteTime(const FileInfo& fileInfo)
{
	uint64_t time = 0;
	switch (fileInfo.fileType_)
	{
	case StaticFileType::Default:
		time = FileSystem::GetLastWriteTime(fileInfo.path_);
		break;
	case StaticFileType::IPC:
		char data;
		ipcStorage_->Lock();
		if (ipcStorage_->GetFile(Effekseer::Tool::StringHelper::ConvertUtf16ToUtf8(fileInfo.path_).c_str(), &data, 1, time) == 0)
			time = 0;
		ipcStorage_->Unlock();
		break;
	}
	return time;
}

void IO::CheckFile(int interval)
{
	while (!isFinishCheckFile_)
	{
		{
			std::lock_guard<std::mutex> lock(mtx_);
			std::vector<FileInfo> removing;

			for (auto& i : fileUpdateDates_)
			{
				auto time = GetFileLastWriteTime(i.first);

				if (time > fileUpdateDates_[i.first] && changedFileInfos_.count(i.first) == 0)
				{
					changedFileInfos_.insert(i.first);
					i.second = time;
				}

				// special case for a material
				// if IPC exists, it should be reload. so call callback forcely.
				else if (i.first.fileType_ == StaticFileType::Default && ipcStorage_ != nullptr)
				{
					auto temp = i.first;
					temp.fileType_ = StaticFileType::IPC;
					if (fileUpdateDates_.count(temp) == 0)
					{
						ipcStorage_->Lock();
						char data = 0;
						if (ipcStorage_->GetFile(Effekseer::Tool::StringHelper::ConvertUtf16ToUtf8(temp.path_).c_str(), &data, 1, time) > 0)
						{
							changedFileInfos_.insert(i.first);
						}
						ipcStorage_->Unlock();
					}
				}

				else if (time == 0)
				{
					removing.emplace_back(i.first);
				}
			}

			// Files are not found
			for (const auto& i : removing)
			{
				changedFileInfos_.insert(i);
				fileUpdateDates_.erase(i);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(interval));
	}
}

void IO::AddCallback(std::shared_ptr<IOCallback> callback)
{
	callbacks_.push_back(callback);
}

} // namespace Effekseer
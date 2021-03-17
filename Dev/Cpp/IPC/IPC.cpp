#define NOMINMAX

#include "IPC.h"
#include <array>
#include <assert.h>
#include <chrono>
#include <memory>
#include <string.h>
#include <string>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#define IPC_IMPLEMENTATION
#include "..//3rdParty/ipc/ipc.h"

#ifndef _WIN32
#include <errno.h>
#endif

namespace IPC
{
class Command_Impl
{
private:
	bool running_ = false;
	ipc_sharedsemaphore coop_ = {};
	ipc_sharedmemory mem_ = {};

public:
	Command_Impl() {}
	virtual ~Command_Impl() { Stop(); }

	bool Start(const char* name, int32_t size)
	{
		auto sem_name_ = std::string(name) + "_sem";
		auto mem_name_ = std::string(name) + "_mem";

		ipc_sem_init(&coop_, const_cast<char*>(sem_name_.c_str()));
		ipc_mem_init(&mem_, const_cast<char*>(mem_name_.c_str()), size + sizeof(int) * 2);

		if (ipc_sem_create(&coop_, 1))
		{
#ifdef _WIN32
			spdlog::warn("Failed ipc_sem_create : {}", name);
#else
			spdlog::warn("Failed ipc_sem_create : {} {} {}", name, errno, strerror(errno));
#endif
			return false;
		}

		ipc_sem_decrement(&coop_);

		if (ipc_mem_open_existing(&mem_) != 0)
		{
#ifdef _WIN32
			spdlog::warn("Failed ipc_mem_open_existing : {}", name);
#else
			spdlog::warn("Failed ipc_mem_open_existing : {} {} {}", name, errno, strerror(errno));
#endif
			if (ipc_mem_create(&mem_))
			{
#ifdef _WIN32
				spdlog::warn("Failed ipc_mem_create : {}", name);
#else
				spdlog::warn("Failed ipc_mem_create : {} {} {}", name, errno, strerror(errno));
#endif
				ipc_sem_close(&coop_);
				ipc_sem_increment(&coop_);
				return false;
			}

			uint32_t* params = reinterpret_cast<uint32_t*>(mem_.data);
			params[0] = size;
			params[1] = 0;
		}

		ipc_sem_increment(&coop_);

		running_ = true;

		return true;
	}

	void Stop()
	{
		if (running_)
		{
			ipc_mem_close(&mem_);
			ipc_sem_close(&coop_);
		}
		running_ = false;
	}

	bool Write(const void* data, int32_t size)
	{
		assert(running_);

		ipc_sem_decrement(&coop_);

		int32_t* params = reinterpret_cast<int32_t*>(mem_.data);
		auto size_all = params[0];
		auto offset = params[1];

		if (size_all < offset + size)
		{
			ipc_sem_increment(&coop_);
			return false;
		}

		memcpy(mem_.data + sizeof(int32_t) * 2 + offset, data, size);
		offset += size;
		params[1] = offset;

		ipc_sem_increment(&coop_);

		return true;
	}

	bool Read(void* data, int32_t size)
	{
		assert(running_);

		ipc_sem_decrement(&coop_);

		int32_t* params = reinterpret_cast<int32_t*>(mem_.data);
		auto size_all = params[0];
		auto offset = params[1];

		if (offset < size)
		{
			ipc_sem_increment(&coop_);
			return false;
		}

		memcpy(reinterpret_cast<uint8_t*>(data), mem_.data + sizeof(int32_t) * 2, size);
		memmove(mem_.data + sizeof(int32_t) * 2, mem_.data + sizeof(int32_t) * 2 + size, offset - size);
		offset -= size;
		params[1] = offset;

		ipc_sem_increment(&coop_);

		return true;
	}
};

CommandQueue::CommandQueue() { impl = std::shared_ptr<Command_Impl>(new Command_Impl()); }

CommandQueue ::~CommandQueue() {}

bool CommandQueue::Start(const char* name, int32_t size) { return impl->Start(name, size); }

void CommandQueue::Stop() { impl->Stop(); }

bool CommandQueue::Write(const void* data, int32_t size) { return impl->Write(data, size); }

bool CommandQueue::Read(void* data, int32_t size) { return impl->Read(data, size); }

bool CommandQueue::Enqueue(CommandData* data)
{
	if (data == nullptr)
		return false;

	return Write(data, sizeof(CommandData));
}

bool CommandQueue::Dequeue(CommandData* data)
{
	if (data == nullptr)
		return false;

	return Read(data, sizeof(CommandData));
}

class KeyValueFileStorage_Impl
{
private:
	bool running_ = false;
	ipc_sharedsemaphore coop_ = {};
	ipc_sharedmemory mem_ = {};

	static const int32_t fileSize = 1024 * 256;
	static const int32_t fileCount = 128;

	struct Key
	{
		int32_t count = 0;
		char key[260];
		uint64_t timestamp = 0;
		int32_t offset;
		int32_t size = 0;
	};

	struct Header
	{
		std::array<Key, fileCount> keys;
	};

public:
	KeyValueFileStorage_Impl() {}
	virtual ~KeyValueFileStorage_Impl() { Stop(); }

	bool Start(const char* name)
	{
		auto size = sizeof(Header) + fileCount * fileSize;

		auto sem_name_ = std::string(name) + "_sem";
		auto mem_name_ = std::string(name) + "_mem";

		ipc_sem_init(&coop_, const_cast<char*>(sem_name_.c_str()));
		ipc_mem_init(&mem_, const_cast<char*>(mem_name_.c_str()), size + sizeof(int) * 2);

		if (ipc_sem_create(&coop_, 1))
		{
#ifdef _WIN32
			spdlog::warn("Failed ipc_sem_create : {}", name);
#else
			spdlog::warn("Failed ipc_sem_create : {} {} {}", name, errno, strerror(errno));
#endif
			return false;
		}

		ipc_sem_decrement(&coop_);

		if (ipc_mem_open_existing(&mem_))
		{
#ifdef _WIN32
			spdlog::warn("Failed ipc_mem_open_existing : {}", name);
#else
			spdlog::warn("Failed ipc_mem_open_existing : {} {} {}", name, errno, strerror(errno));
#endif

			if (ipc_mem_create(&mem_))
			{
#ifdef _WIN32
				spdlog::warn("Failed ipc_mem_create : {}", name);
#else
				spdlog::warn("Failed ipc_mem_create : {} {} {}", name, errno, strerror(errno));
#endif
				ipc_sem_close(&coop_);
				ipc_sem_increment(&coop_);
				return false;
			}

			uint32_t* params = reinterpret_cast<uint32_t*>(mem_.data);
			params[0] = static_cast<uint32_t>(size);
			params[1] = 0;
		}

		ipc_sem_increment(&coop_);

		running_ = true;

		return true;
	}

	void Stop()
	{
		if (running_)
		{
			ipc_mem_close(&mem_);
			ipc_sem_close(&coop_);
		}
		running_ = false;
	}

	bool AddRef(const char* key)
	{
		auto header = reinterpret_cast<Header*>(mem_.data);
		auto key_ = std::string(key);

		for (size_t i = 0; i < fileCount; i++)
		{
			if (header->keys[i].count > 0 && std ::string(header->keys[i].key) == key_)
			{
				header->keys[i].count += 1;
				return true;
			}
		}

		for (size_t i = 0; i < fileCount; i++)
		{
			if (header->keys[i].count == 0)
			{
				memcpy(header->keys[i].key, key, strlen(key) + 1);
				header->keys[i].key[strlen(key)] = 0;
				header->keys[i].count += 1;
				header->keys[i].offset = static_cast<int32_t>(sizeof(Header) + fileSize * i);
				return true;
			}
		}

		return false;
	}

	bool ReleaseRef(const char* key)
	{
		auto header = reinterpret_cast<Header*>(mem_.data);
		auto key_ = std::string(key);

		for (size_t i = 0; i < fileCount; i++)
		{
			if (header->keys[i].count > 0 && std ::string(header->keys[i].key) == key_)
			{
				header->keys[i].count -= 1;
				return true;
			}
		}

		return false;
	}

	bool UpdateFile(const char* key, const void* data, int32_t size)
	{
		int64_t timestamp = std::chrono::system_clock::now().time_since_epoch().count();

		if (size > fileSize)
		{
			return false;
		}

		auto header = reinterpret_cast<Header*>(mem_.data);
		auto key_ = std::string(key);

		for (size_t i = 0; i < fileCount; i++)
		{
			if (header->keys[i].count > 0 && std ::string(header->keys[i].key) == key_)
			{
				header->keys[i].offset = static_cast<int32_t>(sizeof(Header) + fileSize * i);
				header->keys[i].size = size;
				header->keys[i].timestamp = timestamp;
				memcpy(mem_.data + header->keys[i].offset, data, size);
				return true;
			}
		}

		return false;
	}

	int32_t GetFile(const char* key, void* data, int32_t size, uint64_t& timestamp)
	{
		auto header = reinterpret_cast<Header*>(mem_.data);
		auto key_ = std::string(key);

		for (size_t i = 0; i < fileCount; i++)
		{
			if (header->keys[i].count > 0 && std::string(header->keys[i].key) == key_)
			{
				int32_t s = std::min(size, header->keys[i].size);
				memcpy(data, mem_.data + header->keys[i].offset, s);
				timestamp = header->keys[i].timestamp;
				return s;
			}
		}

		return 0;
	}

	void Lock() { ipc_sem_decrement(&coop_); }
	void Unlock() { ipc_sem_increment(&coop_); }
};

KeyValueFileStorage::KeyValueFileStorage() { impl = std::shared_ptr<KeyValueFileStorage_Impl>(new KeyValueFileStorage_Impl()); }
KeyValueFileStorage ::~KeyValueFileStorage() {}

bool KeyValueFileStorage::Start(const char* name) { return impl->Start(name); }

void KeyValueFileStorage::Stop() { impl->Stop(); }

bool KeyValueFileStorage::AddRef(const char* key) { return impl->AddRef(key); }

bool KeyValueFileStorage::ReleaseRef(const char* key) { return impl->ReleaseRef(key); }

void KeyValueFileStorage::UpdateFile(const char* key, const void* data, int32_t size) { impl->UpdateFile(key, data, size); }
int32_t KeyValueFileStorage::GetFile(const char* key, void* data, int32_t size, uint64_t& timestamp)
{
	return impl->GetFile(key, data, size, timestamp);
}
void KeyValueFileStorage::Lock() { impl->Lock(); }
void KeyValueFileStorage::Unlock() { impl->Unlock(); }

} // namespace IPC

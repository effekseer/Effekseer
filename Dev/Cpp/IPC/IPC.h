
#pragma once

#include <array>
#include <memory>
#include <stdint.h>
#include <string.h>

namespace IPC
{

class Command_Impl;

class KeyValueFileStorage_Impl;

enum class CommandType : int32_t
{
	OpenOrCreateMaterial,
	OpenMaterial,
	NotifyUpdate,
	Terminate,
};

struct CommandData
{
	CommandType Type;

	union {
		std::array<char, 260> str;
	};

	bool SetStr(const char* s)
	{

		auto len = strlen(s);
		if (len > str.size() - 1)
			return false;

		memcpy(str.data(), s, len);
		str[len] = 0;
		return true;
	}
};

/**
	@brief	command queue between processes
*/
class CommandQueue
{
	std::shared_ptr<Command_Impl> impl = nullptr;
	bool Write(const void* data, int32_t size);
	bool Read(void* data, int32_t size);

public:
	CommandQueue();
	virtual ~CommandQueue();

	bool Start(const char* name, int32_t size);
	void Stop();
	bool Enqueue(CommandData* data);
	bool Dequeue(CommandData* data);
};

/**
	@brief	a storege between processes
*/
class KeyValueFileStorage
{
	std::shared_ptr<KeyValueFileStorage_Impl> impl = nullptr;

public:
	KeyValueFileStorage();
	virtual ~KeyValueFileStorage();

	bool Start(const char* name);
	void Stop();

	/**
		@brief allocate or increase a reference counter
	*/
	bool AddRef(const char* key);

	/**
		@brief deallocate or descrese a reference counter
	*/
	bool ReleaseRef(const char* key);

	void UpdateFile(const char* key, const void* data, int32_t size);
	int32_t GetFile(const char* key, void* data, int32_t size, uint64_t& timestamp);
	void Lock();
	void Unlock();
};

} // namespace IPC

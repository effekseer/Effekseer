
#pragma once

#include <atomic>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstddef>
#include <dlfcn.h>
#endif

/**
@brief	A class to controll DLL
*/
class DynamicLinkLibrary
{
private:
	mutable std::atomic<int32_t> reference_;

#if _WIN32
	HMODULE dll_;
#else
	void* dll_ = nullptr;
#endif

public:
	DynamicLinkLibrary();

	~DynamicLinkLibrary();

	void Reset();

	/**
		@brief load a dll
	*/
	bool Load(const char* path);

	/**
		@brief get a function pointer
	*/
	template <typename T>
	T GetProc(const char* name)
	{
#if _WIN32
		void* pProc = ::GetProcAddress(dll_, name);
#else
		void* pProc = dlsym(dll_, name);
#endif
		if (pProc == nullptr)
		{
			return nullptr;
		}
		return (T)(pProc);
	}

	int AddRef();

	int GetRef();

	int Release();
};

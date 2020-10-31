#include "DynamicLinkLibrary.h"
#include <assert.h>

DynamicLinkLibrary::DynamicLinkLibrary()
	: reference_(1)
{
	dll_ = nullptr;
}

DynamicLinkLibrary ::~DynamicLinkLibrary()
{
	Reset();
}

void DynamicLinkLibrary::Reset()
{
	if (dll_ != nullptr)
	{
#if _WIN32
		::FreeLibrary(dll_);
#else
		dlclose(dll_);
#endif
		dll_ = nullptr;
	}
}

bool DynamicLinkLibrary::Load(const char* path)
{
	Reset();

#if _WIN32
	dll_ = ::LoadLibraryA(path);
#else
	dll_ = dlopen(path, RTLD_LAZY);
#endif
	if (dll_ == nullptr)
		return false;

	return true;
}

int DynamicLinkLibrary::AddRef()
{
	std::atomic_fetch_add_explicit(&reference_, 1, std::memory_order_consume);
	return reference_;
}

int DynamicLinkLibrary::GetRef()
{
	return reference_;
}

int DynamicLinkLibrary::Release()
{
	assert(reference_ > 0);
	bool destroy = std::atomic_fetch_sub_explicit(&reference_, 1, std::memory_order_consume) == 1;
	if (destroy)
	{
		delete this;
		return 0;
	}

	return reference_;
}

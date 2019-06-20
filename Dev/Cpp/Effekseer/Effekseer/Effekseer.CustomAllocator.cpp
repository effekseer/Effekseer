#include "Effekseer.CustomAllocator.h"

namespace Effekseer
{

void* EFK_STDCALL InternalMalloc(unsigned int size) { return (void*)new char*[size]; }

void EFK_STDCALL InternalFree(void* p, unsigned int size)
{
	char* pData = (char*)p;
	delete[] pData;
}

MallocFunc mallocFunc_ = InternalMalloc;

FreeFunc freeFunc_ = InternalFree;

MallocFunc GetMallocFunc() { return mallocFunc_; }

void SetMallocFunc(MallocFunc func) { mallocFunc_ = func; }

FreeFunc GetFreeFunc() { return freeFunc_; }

void SetFreeFunc(FreeFunc func) { freeFunc_ = func; }

} // namespace Effekseer
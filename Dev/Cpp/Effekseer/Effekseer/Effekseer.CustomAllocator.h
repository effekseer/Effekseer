
#ifndef __EFFEKSEER_CUSTOM_ALLOCATOR_H__
#define __EFFEKSEER_CUSTOM_ALLOCATOR_H__

#include "Effekseer.Base.Pre.h"
#include <list>
#include <map>
#include <new>
#include <set>
#include <vector>

namespace Effekseer
{
/**
	@brief
	\~English get an allocator
	\~Japanese メモリ確保関数を取得する。
*/
MallocFunc GetMallocFunc();

/**
	\~English specify an allocator
	\~Japanese メモリ確保関数を設定する。
*/
void SetMallocFunc(MallocFunc func);

/**
	@brief
	\~English get a deallocator
	\~Japanese メモリ破棄関数を取得する。
*/
FreeFunc GetFreeFunc();

/**
	\~English specify a deallocator
	\~Japanese メモリ破棄関数を設定する。
*/
void SetFreeFunc(FreeFunc func);

template <class T> struct CustomAllocator
{
	using value_type = T;

	CustomAllocator() {}

	template <class U> CustomAllocator(const CustomAllocator<U>&) {}

	T* allocate(std::size_t n) { return reinterpret_cast<T*>(GetMallocFunc()(sizeof(T) * n)); }
	void deallocate(T* p, std::size_t n) { GetFreeFunc()(p, sizeof(T) * n); }
};

template <class T, class U> bool operator==(const CustomAllocator<T>&, const CustomAllocator<U>&) { return true; }

template <class T, class U> bool operator!=(const CustomAllocator<T>&, const CustomAllocator<U>&) { return false; }

template <class T> using CustomVector = std::vector<T, CustomAllocator<T>>;
template <class T> using CustomList = std::list<T, CustomAllocator<T>>;
template <class T> using CustomSet = std::set<T, std::less<T>, CustomAllocator<T>>;
template <class T, class U> using CustomMap = std::map<T, U, std::less<T>, CustomAllocator<std::pair<const T, U>>>;

} // namespace Effekseer

#endif // __EFFEKSEER_BASE_PRE_H__
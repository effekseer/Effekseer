#pragma once

#include <atomic>

// Callbacks may run on a GPU worker thread. Report failure on the test thread,
// after the platform has waited for GPU work and released its resources.
namespace TestDiagnostics
{
inline std::atomic<unsigned int> errorCount{0};

inline void ReportError()
{
	errorCount.fetch_add(1, std::memory_order_relaxed);
}

inline void Reset()
{
	errorCount.store(0, std::memory_order_relaxed);
}

inline unsigned int GetErrorCount()
{
	return errorCount.load(std::memory_order_relaxed);
}
}

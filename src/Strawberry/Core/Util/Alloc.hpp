#pragma once

#ifdef STRAWBERRY_TARGET_WINDOWS
	#include <malloc.h>
#else
	#include <cstdlib>
#endif


namespace Strawberry::Core
{
#ifdef STRAWBERRY_TARGET_WINDOWS
	void* AlignedAlloc(size_t alignment, size_t size) noexcept
	{
		Assert(size % alignment == 0);
		void* ptr = _aligned_malloc(size, alignment);
		AssertNEQ(ptr, nullptr);
		return ptr;
	}

	void AlignedFree(void* ptr) noexcept
	{
		AssertNEQ(ptr, nullptr);
		_aligned_free(ptr);
	}
#else
	void* AlignedAlloc(size_t alignment, size_t size) noexcept
	{
		Assert(size % alignment == 0);
		void* ptr =  std::aligned_alloc(alignment, size);
		AssertNEQ(ptr, nullptr);
		return ptr;
	}

	void AlignedFree(void* ptr) noexcept
	{
		AssertNEQ(ptr, nullptr);
		std::free(ptr);
	}
#endif
}

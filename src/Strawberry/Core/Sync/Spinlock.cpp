#include "Spinlock.hpp"


namespace Strawberry::Core
{
	void Spinlock::Lock() noexcept
	{
		while (mFlag.test_and_set(std::memory_order::acquire));
	}

	void Spinlock::Unlock() noexcept
	{
		mFlag.clear(std::memory_order_release);
	}
}

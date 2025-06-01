#pragma once
// Standard Library
#include <atomic>


namespace Strawberry::Core
{
	/// Class for a wait-free spin lock.
	class Spinlock
	{
	public:
		void Lock() noexcept;
		void Unlock() noexcept;


	private:
		std::atomic_flag mFlag;
	};
}
#pragma once
// Includes
// Strawberry Core
#include "Strawberry/Core/Types/Optional.hpp"
#include "Strawberry/Core/Util/Alloc.hpp"
// Standard Library
#include <atomic>



namespace Strawberry::Core
{
	/// Class for a LockFree Single Writer Single Reader Queue.
	template <typename T, size_t CAPACITY>
	class LockFreeSWSRQueue;

	/// Lock Free Single Write Single Reader Queue with a finite capacity.
	template <typename T, size_t CAPACITY>
	class LockFreeSWSRQueue
	{
	public:
		/// Construct a fresh queue
		LockFreeSWSRQueue()
			: mData(AlignedAlloc(alignof(T), CAPACITY * sizeof(T)))
		{}


		/// Queue is not copyable.
		LockFreeSWSRQueue(const LockFreeSWSRQueue&) = delete;
		/// Queue is not Copy Assignable.
		LockFreeSWSRQueue& operator=(const LockFreeSWSRQueue&) = delete;


		/// Queues are move-constructible.
		LockFreeSWSRQueue(LockFreeSWSRQueue&& x)
			: mHead(x.mHead.load())
			, mTail(x.mTail.load())
			, mData(std::exchange(x.mData, nullptr))
		{}

		/// Queues are not move-assignable.
		LockFreeSWSRQueue& operator=(LockFreeSWSRQueue&&) = delete;


		/// Clean up the data if needed.
		~LockFreeSWSRQueue()
		{
			if (mData)
			{
				AlignedFree(mData);
			}
		}


		/// Push a value onto the queue. Returns true if the value was pushed, and false otherwise.
		bool Push(std::common_reference_with<T> auto value) noexcept
		{
			// Since this thread is the only one to write to mTail, we can use relaxed ordering.
			size_t tail = mTail.load(std::memory_order::relaxed);
			size_t nextTail = (tail + 1) % CAPACITY;

			// If the next position is the head, then we're full.
			if (nextTail == mHead.load(std::memory_order::acquire))
			{
				return false;
			}

			// Construct the value in the position.
			std::construct_at(static_cast<T*>(mData) + tail, std::forward<decltype(value)>(value));
			mTail.store(nextTail, std::memory_order::release);
			return true;
		}


		/// Pop a value from the queue, if there is one.
		Optional<T> Pop() noexcept
		{
			// Since this thread is the only one to write to mTail, we can use relaxed ordering.
			size_t head = mHead.load(std::memory_order::relaxed);
			// If head is equal to tail, then this queue is empty.
			if (head == mTail.load(std::memory_order_acquire))
			{
				return NullOpt;
			}

			// Move the value out of the position.
			T value = std::move(static_cast<T*>(mData)[head]);
			// Clean up the value.
			std::destroy_at(static_cast<T*>(mData) + head);
			// Increment the head.
			mHead.store((head + 1) % CAPACITY, std::memory_order::release);
			// Return the value.
			return value;
		}


	private:
		/// The index of the next position to be pushed to.
		std::atomic<size_t> mHead = 0;
		/// The index of the next position to be popped from.
		std::atomic<size_t> mTail = 0;
		/// A pointer to the underlying data for this queue.
		void* mData;
	};
}

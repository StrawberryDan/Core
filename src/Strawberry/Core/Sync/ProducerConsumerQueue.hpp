#pragma once
// Includes
// Strawberry Core
#include "Strawberry/Core/Types/Optional.hpp"
#include "Strawberry/Core/Util/Alloc.hpp"
// Standard Library
#include <atomic>



namespace Strawberry::Core::LockFree
{
	/// Lock Free Single Writer Single Reader Queue with a finite capacity.
	template <typename T, size_t CAPACITY>
	class SPSCQueue
	{
	public:
		/// Construct a fresh queue
		SPSCQueue()
			: mData(AlignedAlloc(alignof(T), CAPACITY * sizeof(T)))
		{}


		/// Queue is not copyable.
		SPSCQueue(const SPSCQueue&) = delete;
		/// Queue is not Copy Assignable.
		SPSCQueue& operator=(const SPSCQueue&) = delete;


		/// Queues are move-constructible.
		SPSCQueue(SPSCQueue&& x)
 noexcept 			: mHead(x.mHead.load())
			, mTail(x.mTail.load())
			, mData(std::exchange(x.mData, nullptr))
		{}

		/// Queues are not move-assignable.
		SPSCQueue& operator=(SPSCQueue&&) = delete;


		/// Clean up the data if needed.
		~SPSCQueue()
		{
			if (mData)
			{
				while (Pop()) {};
				AlignedFree(mData);
			}
		}


		/// Push a value onto the queue. Returns true if the value was pushed, and false otherwise.
		bool Push(std::common_reference_with<T> auto value) noexcept
		{	ZoneScoped;
			// Since this thread is the only one to write to mTail, we can use relaxed ordering.
			size_t tail = mTail.load(std::memory_order::relaxed);

			// Don't insert if the queue is full.
			if (IsFull())
			{
				return false;
			}

			// Construct the value in the position.
			std::construct_at(static_cast<T*>(mData) + tail, std::forward<decltype(value)>(value));
			// Store the new tail
			mTail.store((tail + 1) % CAPACITY, std::memory_order::release);
			// Increment the size counter
			mSize.fetch_add(1, std::memory_order_release);
			return true;
		}


		/// Pop a value from the queue if there is one.
		Optional<T> Pop() noexcept
		{	ZoneScoped;
			// Since this thread is the only one to write to mTail, we can use relaxed ordering.
			size_t head = mHead.load(std::memory_order::relaxed);
			// Can't pop if the queue is empty.
			if (IsEmpty())
			{
				return NullOpt;
			}

			// Move the value out of the position.
			T value = std::move(static_cast<T*>(mData)[head]);
			// Clean up the value.
			std::destroy_at(static_cast<T*>(mData) + head);
			// Increment the head.
			mHead.store((head + 1) % CAPACITY, std::memory_order::release);
			// Decrease the size of the queue
			mSize.fetch_sub(1, std::memory_order_release);
			// Return the value.
			return value;
		}


		[[nodiscard]] bool IsFull() const noexcept
		{
			return mSize.load(std::memory_order::acquire) == CAPACITY;
		}


		[[nodiscard]] bool IsEmpty() const noexcept
		{
			return mSize.load(std::memory_order::acquire) == 0;
		}


	private:
		/// The number of items stored in this queue
		std::atomic<size_t> mSize = 0;
		/// The index of the next position to be pushed to.
		std::atomic<size_t> mHead = 0;
		/// The index of the next position to be popped from.
		std::atomic<size_t> mTail = 0;
		/// A pointer to the underlying data for this queue.
		void* mData;
	};
}

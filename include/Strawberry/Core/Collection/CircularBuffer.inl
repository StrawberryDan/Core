#include "CircularBuffer.hpp"



namespace Strawberry::Core::Collection
{




	template<typename T, size_t S> requires std::movable<T> || std::copyable<T>
	CircularBuffer<T, S>::CircularBuffer()
		: mHead(0), mTail(0), mSize(0), mItems()
	{}



	template<typename T, size_t S> requires std::movable<T> || std::copyable<T>
	CircularBuffer<T, S>::CircularBuffer(const CircularBuffer& other) requires std::copyable<T>
		: mHead(other.mHead), mTail(other.mTail), mSize(other.mSize), mItems(other.mItems)
	{}



	template<typename T, size_t S> requires std::movable<T> || std::copyable<T>
	CircularBuffer<T, S>&	CircularBuffer<T, S>::operator=(const CircularBuffer& other) requires std::copyable<T>
	{
		mHead = other.mHead;
		mTail = other.mTail;
		mSize = other.mSize;
		mItems = other.mItems;
		return *this;
	}



	template<typename T, size_t S> requires std::movable<T> || std::copyable<T>
	CircularBuffer<T, S>::CircularBuffer(CircularBuffer&& other) requires std::movable<T>
        : mHead(Replace(other.mHead, 0))
		, mTail(Replace(other.mTail, 0))
		, mSize(Replace(other.mSize, 0))
		, mItems(Take(other.mItems))
	{}



	template<typename T, size_t S> requires std::movable<T> || std::copyable<T>
	CircularBuffer<T, S>& CircularBuffer<T, S>::operator=(CircularBuffer&& other) requires std::movable<T>
	{
		if (this != &other)
		{
			mHead = Replace(other.mHead, 0);
			mTail = Replace(other.mTail, 0);
			mSize = Replace(other.mSize, 0);
			mItems = Take(other.mItems);
		}

		return *this;
	}



	template<typename T, size_t S> requires std::movable<T> || std::copyable<T>
	void CircularBuffer<T, S>::Push(T object)
	{
		Assert(RemainingCapacity() > 0);
		mItems[mTail] = std::move(object);
		mTail += 1;
		mSize += 1;
		if (mTail >= Capacity())
			mTail = mTail % Capacity();
	}



	template<typename T, size_t S> requires std::movable<T> || std::copyable<T>
	Option<T> CircularBuffer<T, S>::Pop()
	{
		if (Size() == 0)
		{
			return {};
		}
		else
		{
			auto item = std::move(mItems[mHead]);
			mHead += 1;
			mSize -= 1;
			if (mHead >= Capacity())
				mHead = mHead % Capacity();
			return item;
		}
	}



	template<typename T, size_t S> requires std::movable<T> || std::copyable<T>
	std::vector<T> CircularBuffer<T, S>::PopAll()
	{
		std::vector<T> items;
		mItems.reserve(Size());

		while (Size() > 0)
		{
			items.push_back(std::move(*Pop()));
		}

		mHead = 0;
		mTail = 0;
		return items;
	}







	template<typename T> requires std::movable<T> || std::copyable<T>
	DynamicCircularBuffer<T>::DynamicCircularBuffer()
		: mHead(0), mTail(0), mSize(0), mItems(8, {})
	{
	}



	template<typename T> requires std::movable<T> || std::copyable<T>
	DynamicCircularBuffer<T>::DynamicCircularBuffer(const DynamicCircularBuffer<T>& other) requires std::copyable<T>
			: mHead(other.mHead), mTail(other.mTail), mSize(other.mSize), mItems(other.mItems)
	{}



	template<typename T> requires std::movable<T> || std::copyable<T>
	DynamicCircularBuffer<T>& DynamicCircularBuffer<T>::operator=(const DynamicCircularBuffer<T>& other) requires std::copyable<T>
	{
		mHead = other.mHead;
		mTail = other.mTail;
		mSize = other.mSize;
		mItems = other.mItems;
		return *this;
	}



	template<typename T> requires std::movable<T> || std::copyable<T>
	DynamicCircularBuffer<T>::DynamicCircularBuffer(DynamicCircularBuffer<T>&& other) requires std::movable<T>
			: mHead(Replace(other.mHead, 0))
			, mTail(Replace(other.mTail, 0))
			, mSize(Replace(other.mSize, 0))
			, mItems(Take(other.mItems))
	{}



	template<typename T> requires std::movable<T> || std::copyable<T>
	DynamicCircularBuffer<T>& DynamicCircularBuffer<T>::operator=(DynamicCircularBuffer<T>&& other) requires std::movable<T>
	{
		if (this != &other)
		{
			mHead = Replace(other.mHead, 0);
			mTail = Replace(other.mTail, 0);
			mSize = Replace(other.mSize, 0);
			mItems = Take(other.mItems);
		}

		return *this;
	}



	template<typename T> requires std::movable<T> || std::copyable<T>
	void DynamicCircularBuffer<T>::Push(T object)
	{
		if (RemainingCapacity() == 0)
		{
			std::vector<Option<T>> newItems(2 * Capacity(), {});
			size_t i = 0;
			while (Size() > 0)
			{
				newItems[i] = *Pop();
				i += 1;
			}

			mHead = 0;
			mTail = i - 1;
			mItems = std::move(newItems);
		}

		mItems[mTail] = std::move(object);
		mTail += 1;
		mSize += 1;
		if (mTail >= Capacity())
			mTail = mTail % Capacity();
	}



	template<typename T> requires std::movable<T> || std::copyable<T>
	Option<T> DynamicCircularBuffer<T>::Pop()
	{
		if (Size() == 0)
		{
			return {};
		}
		else
		{
			auto item = std::move(mItems[mHead]);
			mHead += 1;
			mSize -= 1;
			if (mHead >= Capacity())
				mHead = mHead % Capacity();
			return item;
		}
	}



	template<typename T> requires std::movable<T> || std::copyable<T>
	std::vector<T> DynamicCircularBuffer<T>::PopAll()
	{
		std::vector<T> items;
		mItems.reserve(Size());

		while (Size() > 0)
		{
			items.push_back(std::move(*Pop()));
		}

		mHead = 0;
		mTail = 0;
		return items;
	}
}
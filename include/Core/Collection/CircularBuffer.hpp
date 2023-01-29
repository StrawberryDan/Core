#pragma once



#include <concepts>
#include "Core/Option.hpp"



namespace Strawberry::Core::Collection
{
	template <typename T, size_t S> requires std::movable<T> || std::copyable<T>
	class CircularBuffer
	{
	public:
		CircularBuffer();
		CircularBuffer(const CircularBuffer& other) requires std::copyable<T>;
		CircularBuffer& operator=(const CircularBuffer& other) requires std::copyable<T>;
		CircularBuffer(CircularBuffer&& other) requires std::movable<T>;
		CircularBuffer& operator=(CircularBuffer&& other) requires std::movable<T>;



		void Push(T object);
		Option<T> Pop();
		std::vector<T> PopAll();



		size_t Size() const { return mSize; }
		size_t Capacity() const { return S; }
		size_t RemainingCapacity() const { return Capacity() - Size(); }



	private:
		size_t mHead, mTail, mSize;
		std::array<Option<T>, S> mItems;
	};





	template <typename T> requires std::movable<T> || std::copyable<T>
	class DynamicCircularBuffer
	{
	public:
		DynamicCircularBuffer();
		DynamicCircularBuffer(const DynamicCircularBuffer& other) requires std::copyable<T>;
		DynamicCircularBuffer& operator=(const DynamicCircularBuffer& other) requires std::copyable<T>;
		DynamicCircularBuffer(DynamicCircularBuffer&& other) requires std::movable<T>;
		DynamicCircularBuffer& operator=(DynamicCircularBuffer&& other) requires std::movable<T>;



		void Push(T object);
		Option<T> Pop();
		std::vector<T> PopAll();



		size_t Size() const { return mSize; }
		size_t Capacity() const { return mItems.size(); }
		size_t RemainingCapacity() const { return Capacity() - Size(); }



	private:
		size_t mHead, mTail, mSize;
		std::vector<Option<T>> mItems;
	};
}



#include "Core/Collection/CircularBuffer.inl"
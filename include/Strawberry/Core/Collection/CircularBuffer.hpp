#pragma once


#include "Strawberry/Core/Math/Periodic.hpp"
#include "Strawberry/Core/Util/Optional.hpp"

#include <memory>


namespace Strawberry::Core::Collection
{
	template <typename T>
	class CircularBuffer
	{
	public:
		explicit CircularBuffer(size_t capacity)
			: mData(capacity)
			, mHead(Capacity(), 0)
			, mTail(Capacity(), 0)
			, mSize(0)
		{}


		void Push(T value)
		{
			if (AtCapacity()) { mData[*mHead++].Reset(); }

			mData[*(mTail++)].Emplace(std::move(value));
			if (!AtCapacity()) mSize += 1;
		}

		Optional<T> Pop()
		{
			if (Size() == 0) return {};
			auto value = std::move(*mData[*mHead]);
			mHead++;
			mSize -= 1;
			return value;
		}


		[[nodiscard]] size_t Size() const { return mSize; }


		[[nodiscard]] bool Empty() const { return Size() == 0; }


		[[nodiscard]] size_t Capacity() const { return mData.size(); }


		[[nodiscard]] bool AtCapacity() const { return Capacity() == Size(); }


		void Clear()
		{
			while (!Empty()) Pop();
		}


	private:
		std::vector<Optional<T>>      mData;
		Math::DynamicPeriodic<size_t> mHead;
		Math::DynamicPeriodic<size_t> mTail;
		size_t                        mSize;
	};


	template <typename T>
	class DynamicCircularBuffer
	{
	public:
		DynamicCircularBuffer()
			: mData(16)
			, mHead(Capacity(), 0)
			, mTail(Capacity(), 0)
			, mSize(0)
		{}


		void Push(T value)
		{
			if (AtCapacity()) { Resize(2 * Capacity()); }

			mData[*(mTail++)].Emplace(std::move(value));
			if (!AtCapacity()) mSize += 1;
		}

		Optional<T> Pop()
		{
			if (Size() == 0) return {};
			auto value  = std::move(*mData[*mHead++]);
			mSize      -= 1;
			return value;
		}


		[[nodiscard]] size_t Size() const { return mSize; }


		[[nodiscard]] bool Empty() const { return Size() == 0; }


		[[nodiscard]] size_t Capacity() const { return mData.size(); }


		[[nodiscard]] bool AtCapacity() const { return Capacity() == Size(); }


		void Clear()
		{
			while (!Empty()) Pop();
		}


	private:
		void Resize(size_t newSize)
		{
			std::vector<T> newData;
			newData.reserve(newSize);

			while (!Empty()) newData.emplace_back(std::move(Pop().Unwrap()));

			mHead = Math::DynamicPeriodic(newSize, (size_t) 0);
			mTail = Math::DynamicPeriodic(newSize, (size_t) 0);
			mSize = 0;

			mData = std::vector<Optional<T>>(newSize, Option<T>());
			for (auto& value : newData) Push(std::move(value));
		}


	private:
		std::vector<Optional<T>>      mData;
		Math::DynamicPeriodic<size_t> mHead;
		Math::DynamicPeriodic<size_t> mTail;
		size_t                        mSize;
	};
} // namespace Strawberry::Core::Collection

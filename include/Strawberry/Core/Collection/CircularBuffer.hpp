#pragma once



#include "Strawberry/Core/Math/Periodic.hpp"
#include "Strawberry/Core/Option.hpp"
#include "Strawberry/Core/Option.hpp"

#include <memory>



namespace Strawberry::Core::Collection
{
	template <typename T>
	class CircularBuffer
	{
	public:
		CircularBuffer()
			: mData(16)
			, mHead(Capacity(), 0)
			, mTail(Capacity(), 0)
			, mSize(0)
		{}


		void Push(T value)
		{
			if (AtCapacity())
			{
				mData[*mHead++].Reset();
			}

			mData[*(mTail++)].Emplace(std::move(value));
			if (!AtCapacity()) mSize += 1;
		}


		Option<T> Pop()
		{
			if (Size() == 0) return {};
			auto value = std::move(*mData[*mHead]);
			mHead++;
			mSize -= 1;
			return value;
		}


		size_t Size()       const { return mSize; }
		bool   Empty()      const { return Size() == 0; }
		size_t Capacity()   const { return mData.size(); }
		bool   AtCapacity() const { return Capacity() == Size(); }
		void   Clear()            { while (!Empty()) Pop(); }


	private:
		std::vector<Option<T>>        mData;
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
			if (AtCapacity())
			{
				Resize(2 * Capacity());
			}

			mData[*(mTail++)].Emplace(std::move(value));
			if (!AtCapacity()) mSize += 1;
		}


		Option<T> Pop()
		{
			if (Size() == 0) return {};
			auto value = std::move(*mData[*mHead++]);
			mSize -= 1;
			return value;
		}


		size_t Size()       const { return mSize; }
		bool   Empty()      const { return Size() == 0; }
		size_t Capacity()   const { return mData.size(); }
		bool   AtCapacity() const { return Capacity() == Size(); }
		void   Clear()            { while (!Empty()) Pop(); }


	private:
		void Resize(size_t newSize)
		{
			std::vector<T> newData;
			newData.reserve(newSize);

			while (!Empty()) newData.emplace_back(std::move(Pop().Unwrap()));

			mHead = Math::DynamicPeriodic(newSize, (size_t)0);
			mTail = Math::DynamicPeriodic(newSize, (size_t)0);
			mSize = 0;

			for (auto& value: newData) Push(std::move(value));
			mData.resize(newSize);
		}


	private:
		std::vector<Option<T>>        mData;
		Math::DynamicPeriodic<size_t> mHead;
		Math::DynamicPeriodic<size_t> mTail;
		size_t                        mSize;
	};
}

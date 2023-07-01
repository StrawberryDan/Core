#pragma once



#include "Strawberry/Core/Math/Periodic.hpp"
#include "Strawberry/Core/Uninitialised.hpp"
#include "Strawberry/Core/Option.hpp"

#include <memory>



namespace Strawberry::Core::IO
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
				std::destroy_at(&mData[*mHead++]);
			}

			std::construct_at(&*mData[*(mTail++)], std::move(value));
			if (!AtCapacity()) mSize += 1;
		}


		Option<T> Pop()
		{
			if (Size() == 0) return {};
			auto value = std::move(*mData[*mHead]);
			std::destroy_at(&*mData[*mHead]);
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
		std::vector<Uninitialised<T>> mData;
		Math::DynamicPeriodic<size_t> mHead;
		Math::DynamicPeriodic<size_t> mTail;
		size_t                        mSize;
	};
}

#pragma once


#include <utility>

#include "Consumer.hpp"
#include "Producer.hpp"
#include "Strawberry/Core/Collection/CircularBuffer.hpp"


namespace Strawberry::Core::IO
{
	template <typename T>
	class Buffer
		: public Consumer<T>
		, public Producer<T>
	{
	public:
		bool Feed(T&& t) override
		{
			if (mCapacity && mBuffer.Size() > *mCapacity)
			{
				return false;
			}

			mBuffer.Push(std::move(t));
			return true;
		}


		Option<T> Receive() override
		{
			return mBuffer.Pop();
		}


		const Option<size_t>& GetCapacity()
		{
			return mCapacity;
		}


		void SetCapacity(Option<size_t> capacity)
		{
			mCapacity = std::move(capacity);
		}


	private:
		Option<size_t> 	                     mCapacity;
		Collection::DynamicCircularBuffer<T> mBuffer;
	};
}
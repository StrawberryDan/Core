#pragma once



#include "DynamicByteBuffer.hpp"
#include "Error.hpp"
#include "Standard/Result.hpp"
#include <cstdlib>



namespace Strawberry::Standard::IO
{
	class CircularDynamicByteBuffer
	{
	public:
		CircularDynamicByteBuffer();



		Result<DynamicByteBuffer, Error> Read(size_t len);
		Result<size_t, Error> Write(const DynamicByteBuffer& bytes);



		inline size_t Size() const { return mSize; }
		inline size_t Capacity() const { return mBuffer.Size(); }
		inline size_t RemainingCapacity() const { return Capacity() - Size(); }


	private:
		void Expand();



	private:
		size_t mHead, mTail, mSize;
		DynamicByteBuffer mBuffer;
	};
}
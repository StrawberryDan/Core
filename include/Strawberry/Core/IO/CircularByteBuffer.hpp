#pragma once



#include "ByteBuffer.hpp"
#include "Error.hpp"
#include "Strawberry/Core/Result.hpp"
#include <cstdlib>



namespace Strawberry::Core::IO
{
	template <size_t S>
	class CircularByteBuffer
	{
	public:
		CircularByteBuffer();



		Result<DynamicByteBuffer, Error> Read(size_t len);
		Result<size_t, Error> Write(const DynamicByteBuffer& bytes);



		inline size_t Size() const { return mSize; }
		inline size_t Capacity() const { return S; }
		inline size_t RemainingCapacity() const { return Capacity() - Size(); }

	private:
		size_t mHead, mTail, mSize;
		ByteBuffer<S> mBuffer;
	};



	template<size_t S>
	CircularByteBuffer<S>::CircularByteBuffer()
		: mHead(0), mTail(0), mSize(0), mBuffer()
	{
	}



	template<size_t S>
	Result<DynamicByteBuffer, Error> CircularByteBuffer<S>::Read(size_t len)
	{
		Assert(len <= Capacity());
		if (mSize == 0 || mSize < len)
		{
			return Error::WouldBlock;
		}

		size_t bytesRead = 0;
		auto result = DynamicByteBuffer::Zeroes(len);
		if (mHead < mTail)
		{
			memcpy(result.Data(), mBuffer.Data() + mHead, len);
			mHead = (mHead + len) % Capacity();
			bytesRead += len;
			mSize -= bytesRead;
		}
		else
		{
			memcpy(result.Data(), mBuffer.Data() + mHead, std::min(len, Capacity() - mHead));
			bytesRead += std::min(len, Capacity() - mHead);
			mHead = (mHead + bytesRead) % Capacity();
			mSize -= bytesRead;

			if (bytesRead < len)
			{
				auto remainder = len - bytesRead;
				memcpy(result.Data(), mBuffer.Data(), remainder);
				mHead += remainder;
				bytesRead += remainder;
				mSize -= remainder;
			}
		}

		Assert(bytesRead == len);
		return result;
	}



	template<size_t S>
	Result<size_t, Error> CircularByteBuffer<S>::Write(const DynamicByteBuffer& bytes)
	{
		if (RemainingCapacity() < bytes.Size())
		{
			return Error::BufferOverflow;
		}

		size_t bytesWritten = 0;
		memcpy(mBuffer.Data() + mTail, bytes.Data(), std::min(S - mTail, bytes.Size()));
		bytesWritten += std::min(S - mTail, bytes.Size());
		mTail = (mTail + bytesWritten) % Capacity();
		mSize += bytesWritten;

		if (bytesWritten < bytes.Size())
		{
			size_t remainder = bytes.Size() - bytesWritten;
			memcpy(mBuffer.Data() + mTail, bytes.Data() + bytesWritten, remainder);
			mTail += remainder;
			mSize += remainder;
		}

		return bytesWritten;
	}
}

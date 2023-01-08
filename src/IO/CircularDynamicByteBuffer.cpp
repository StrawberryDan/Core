#include "Standard/IO/CircularDynamicByteBuffer.hpp"



namespace Strawberry::Standard::IO
{
	CircularDynamicByteBuffer::CircularDynamicByteBuffer()
			: mHead(0), mTail(0), mSize(0), mBuffer(DynamicByteBuffer::Zeroes(1024))
	{
	}



	Result<DynamicByteBuffer, Error> CircularDynamicByteBuffer::Read(size_t len)
	{
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



	Result<size_t, Error> CircularDynamicByteBuffer::Write(const DynamicByteBuffer& bytes)
	{
		if (RemainingCapacity() < bytes.Size())
		{
			Expand();
		}

		size_t bytesWritten = 0;
		memcpy(mBuffer.Data() + mTail, bytes.Data(), std::min(Capacity() - mTail, bytes.Size()));
		bytesWritten += std::min(Capacity() - mTail, bytes.Size());
		mTail = (mTail + bytesWritten) % Capacity();
		mSize += bytesWritten;

		if (bytesWritten < bytes.Size())
		{
			size_t remainder = bytes.Size() - bytesWritten;
			memcpy(mBuffer.Data() + mTail, bytes.Data() + bytesWritten, remainder);
			mTail += remainder;
			mSize += remainder;
		}

		if (RemainingCapacity() == 0)
		{
			Expand();
		}

		return bytesWritten;
	}



	void CircularDynamicByteBuffer::Expand()
	{
		DynamicByteBuffer newBuffer = DynamicByteBuffer::Zeroes(Capacity() * 2);

		if (mHead < mTail)
		{
			memcpy(newBuffer.Data(), mBuffer.Data() + mHead, mSize);
		}
		else
		{
			memcpy(newBuffer.Data(), mBuffer.Data() + mHead, std::min(Capacity() - mHead, mSize));
			auto bytesCopied = std::min(Capacity() - mHead, mSize);
			if (bytesCopied < mSize)
			{
				memcpy(newBuffer.Data(), mBuffer.Data(), mSize - bytesCopied);
			}
		}

		mHead = 0;
		mTail = mSize;
		mBuffer = std::move(newBuffer);
	}
}
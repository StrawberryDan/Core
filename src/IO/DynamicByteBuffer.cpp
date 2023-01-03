#include "Standard/IO/DynamicByteBuffer.hpp"





Strawberry::Standard::IO::DynamicByteBuffer::DynamicByteBuffer(size_t capacity)
{
	mData.reserve(capacity);
}



Strawberry::Standard::IO::DynamicByteBuffer::DynamicByteBuffer(const uint8_t* data, size_t len)
	: DynamicByteBuffer(len)
{
	memcpy(mData.data(), data, len);
}



size_t Strawberry::Standard::IO::DynamicByteBuffer::Size() const
{
	return mData.size();
}



uint8_t* Strawberry::Standard::IO::DynamicByteBuffer::Data()
{
	return mData.data();
}



const uint8_t* Strawberry::Standard::IO::DynamicByteBuffer::Data() const
{
	return mData.data();
}



std::strong_ordering
Strawberry::Standard::IO::DynamicByteBuffer::operator<=>(const Strawberry::Standard::IO::DynamicByteBuffer& rhs) const
{
	Assert(Size() == rhs.Size());
	for (int i = 0; i < Size(); i++)
	{
		if ((*this)[i] < rhs[i])
		{
			return std::strong_ordering::less;
		}
		else if ((*this)[i] > rhs[i])
		{
			return std::strong_ordering::greater;
		}
	}

	return std::strong_ordering::equal;
}



std::vector<uint8_t>& Strawberry::Standard::IO::DynamicByteBuffer::AsVector()
{
	return mData;
}



const std::vector<uint8_t>& Strawberry::Standard::IO::DynamicByteBuffer::AsVector() const
{
	return mData;
}


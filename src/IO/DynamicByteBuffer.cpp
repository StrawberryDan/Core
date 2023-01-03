#include "Standard/IO/DynamicByteBuffer.hpp"





Strawberry::Standard::IO::DynamicByteBuffer::DynamicByteBuffer(size_t capacity)
{
	mData.reserve(capacity);
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



std::vector<uint8_t>& Strawberry::Standard::IO::DynamicByteBuffer::AsVector()
{
	return mData;
}



const std::vector<uint8_t>& Strawberry::Standard::IO::DynamicByteBuffer::AsVector() const
{
	return mData;
}

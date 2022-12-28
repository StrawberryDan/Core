#include "Standard/ByteBuffer.hpp"



size_t Strawberry::Standard::ByteBuffer::Size() const
{
	return mBytes.size();
}



uint8_t* Strawberry::Standard::ByteBuffer::Data()
{
	return mBytes.data();
}



const uint8_t* Strawberry::Standard::ByteBuffer::Data() const
{
	return mBytes.data();
}



std::vector<uint8_t>& Strawberry::Standard::ByteBuffer::AsVector()
{
	return mBytes;
}



const std::vector<uint8_t>& Strawberry::Standard::ByteBuffer::AsVector() const
{
	return mBytes;
}

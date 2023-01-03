#include "Standard/IO/ByteBuffer.hpp"



namespace Strawberry::Standard::IO
{
	template<size_t S>
	ByteBuffer<S>::ByteBuffer(const uint8_t* data, size_t len)
	{
		memcpy(mData.data(), data, len);
	}
}
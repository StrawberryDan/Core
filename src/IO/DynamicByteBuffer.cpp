#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"



Strawberry::Core::IO::DynamicByteBuffer Strawberry::Core::IO::DynamicByteBuffer::Zeroes(size_t len)
{
	DynamicByteBuffer result;
	result.mData = std::vector<uint8_t>(len, 0);
	return result;
}



Strawberry::Core::IO::DynamicByteBuffer Strawberry::Core::IO::DynamicByteBuffer::WithCapacity(size_t len)
{
	DynamicByteBuffer result;
	result.mData.reserve(len);
	return result;
}



size_t Strawberry::Core::IO::DynamicByteBuffer::Size() const
{
	return mData.size();
}



uint8_t* Strawberry::Core::IO::DynamicByteBuffer::Data()
{
	return mData.data();
}



const uint8_t* Strawberry::Core::IO::DynamicByteBuffer::Data() const
{
	return mData.data();
}



void Strawberry::Core::IO::DynamicByteBuffer::Reserve(size_t len)
{
	mData.reserve(len);
}



void Strawberry::Core::IO::DynamicByteBuffer::Resize(size_t len)
{
	mData.resize(len);
}



std::strong_ordering
Strawberry::Core::IO::DynamicByteBuffer::operator<=>(const Strawberry::Core::IO::DynamicByteBuffer& rhs) const
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



std::vector<uint8_t>& Strawberry::Core::IO::DynamicByteBuffer::AsVector()
{
	return mData;
}



const std::vector<uint8_t>& Strawberry::Core::IO::DynamicByteBuffer::AsVector() const
{
	return mData;
}



std::string Strawberry::Core::IO::DynamicByteBuffer::AsString() const
{

	std::string string(Size() + 1, 0);
	memcpy(string.data(), Data(), Size());
	return string;
}

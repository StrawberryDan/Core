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



template<>
std::vector<uint8_t> Strawberry::Core::IO::DynamicByteBuffer::AsVector<uint8_t>()
{
	return mData;
}



std::string Strawberry::Core::IO::DynamicByteBuffer::AsString() const
{

	std::string string(Size() + 1, 0);
	memcpy(string.data(), Data(), Size());
	return string;
}


Strawberry::Core::Result<Strawberry::Core::IO::DynamicByteBuffer, Strawberry::Core::IO::Error>
Strawberry::Core::IO::DynamicByteBuffer::Read(size_t len)
{
	if (Size() - mReadCursor < len)
	{
		return Error::EndOfFile;
	}

	return DynamicByteBuffer(Data() + mReadCursor, len);
}


Strawberry::Core::Result<size_t, Strawberry::Core::IO::Error>
Strawberry::Core::IO::DynamicByteBuffer::Write(const Strawberry::Core::IO::DynamicByteBuffer& bytes)
{
	for (auto byte : bytes)
	{
		Push(byte);
	}

	return bytes.Size();
}

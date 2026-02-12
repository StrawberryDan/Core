


#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"

#include "Strawberry/Core/Traits.hpp"

using namespace Strawberry::Core;

int main()
{
	IO::DynamicByteBuffer byteBuffer;

	Reader<IO::DynamicByteBuffer> byteBufferReader(byteBuffer);
	Writer<IO::DynamicByteBuffer> byteBufferWriter(byteBuffer);

	byteBufferWriter.Write(IO::DynamicByteBuffer::FromObjects<char>('u')).Unwrap();

	auto read = byteBufferReader.Read(1).Unwrap();
	AssertEQ(read.Into<char>(), 'u');


	return 0;
}

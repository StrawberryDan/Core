#pragma once
#include "Error.hpp"
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
#include "Types/Result.hpp"


namespace Strawberry::Core
{
	using ReadResult = Result<IO::DynamicByteBuffer, Error>;
	using WriteResult = Result<void, Error>;


	template<typename T>
	class Reader
	{
	public:
	};


	template <typename T>
	class Writer
	{

	};


	template <>
	class Reader<IO::DynamicByteBuffer>
	{
	public:
		Reader(IO::DynamicByteBuffer& buffer)
			: mBuffer(&buffer)
		{}


		ReadResult Read(size_t count) noexcept
		{
			IO::DynamicByteBuffer result = IO::DynamicByteBuffer::WithCapacity(count);

			for (unsigned i = 0; i < count; i++)
			{
				result.Push((*mBuffer)[i + mPosition++]);
			}

			return result;
		}

	private:
		IO::DynamicByteBuffer* mBuffer;
		size_t mPosition = 0;
	};


	template <>
	class Writer<IO::DynamicByteBuffer>
	{
	public:
		Writer(IO::DynamicByteBuffer& buffer)
			: mBuffer(&buffer)
		{}


		WriteResult Write(IO::DynamicByteBuffer&& data)
		{
			mBuffer->Push(data);
			return Success;
		}

	private:
		IO::DynamicByteBuffer* mBuffer;
	};
}

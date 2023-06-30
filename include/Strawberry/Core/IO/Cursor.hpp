#pragma once



#include "Concepts.hpp"

namespace Strawberry::Core::IO
{
	template <Indexable<uint8_t> Container>
	class Cursor
	{
		Cursor(Container container)
			: mPosition(0)
			, mContainer(std::move(container))
		{}



		Result<DynamicByteBuffer, IO::Error> Read(size_t len)
		{
			auto buffer = DynamicByteBuffer::Zeroes(len);
			for (int i = 0; i < len; i++) buffer.Push(mContainer[mPosition++]);
			return buffer;
		}



		Result<DynamicByteBuffer, Error> Read(size_t len) requires SizedContainer<Container> || std::same_as<Container, std::vector<uint8_t>>
		{
			if (mPosition + len >= Size()) return Error::OverRead;
			auto buffer = DynamicByteBuffer::Zeroes(len);
			for (int i = 0; i < len; i++) buffer.Push(mContainer[mPosition++]);
			return buffer;
		}


		void Write(DynamicByteBuffer bytes) requires ReferenceIndexable<Container, uint8_t>
		{
			for (auto byte : bytes)
			{
				mContainer[mPosition++] = bytes;
			}
		}


		void Write(DynamicByteBuffer bytes) requires ReferenceIndexable<Container, uint8_t> && (SizedContainer<Container> || std::same_as<Container, std::vector<uint8_t>>)
		{
			Assert(mPosition + bytes.Size() < Size());
			for (auto byte : bytes)
			{
				mContainer[mPosition++] = bytes;
			}
		}


		size_t GetPosition() const { return mPosition; }
		void SetPosition(size_t pos) { mPosition = pos; }


		size_t Size() const requires SizedContainer<Container>
		{
			return mContainer.Size();
		}


		size_t Size() const requires std::same_as<Container, std::vector<uint8_t>>
		{
			return mContainer.size();
		}



	private:
		size_t    mPosition;
		Container mContainer;
	};



	/// Type Deduction Guide.
	template <Indexable<uint8_t> Container>
	Cursor(Container) -> Cursor<Container>;
}
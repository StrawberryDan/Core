#pragma once



#include "Standard/Assert.hpp"
#include "Standard/Result.hpp"
#include <compare>
#include <concepts>
#include <cstdint>
#include <vector>



namespace Strawberry::Standard::IO
{
	template <size_t S>
	class ByteBuffer;



	class DynamicByteBuffer
	{
	public:
		// Static Methods
		static DynamicByteBuffer Zeroes(size_t len);
		static DynamicByteBuffer WithCapacity(size_t len);



		// Constructors
		DynamicByteBuffer() = default;
		template <typename T>
		DynamicByteBuffer(const T* data, size_t len);
		template <typename T>
		DynamicByteBuffer(const T& object);




		// Pushing Data
		template <typename T>
		void Push(const T* data, size_t count);

		template <typename T>
		void Push(const T& data);

		template <typename T>
		void Push(const std::vector<T>& data);

		inline void Push(const IO::DynamicByteBuffer& bytes) { Push(bytes.Data(), bytes.Size()); }

		template <size_t S>
		inline void Push(const IO::ByteBuffer<S>& bytes) { Push(bytes.Data(), bytes.Size()); }



		// Accessors
		size_t Size() const;

		      uint8_t* Data();
		const uint8_t* Data() const;

		      uint8_t& operator[](size_t i)       { return mData[i]; }
		const uint8_t& operator[](size_t i) const { return mData[i]; }



		// Sizing
		void Reserve(size_t len);
		void Resize(size_t len);



		// Comparison
		std::strong_ordering operator<=>(const DynamicByteBuffer& rhs) const;
		bool operator==(const DynamicByteBuffer& rhs) const = default;
		bool operator!=(const DynamicByteBuffer& rhs) const = default;
		bool operator >(const DynamicByteBuffer& rhs) const = default;
		bool operator <(const DynamicByteBuffer& rhs) const = default;
		bool operator>=(const DynamicByteBuffer& rhs) const = default;
		bool operator<=(const DynamicByteBuffer& rhs) const = default;



		// Casting
		template <typename T> requires std::copyable<T> || std::movable<T>
		T Into() const;

		template <typename T>
		void Into(T& data) const;

		template <size_t S>
		ByteBuffer<S> AsStatic() const;

		template <size_t N>
		std::array<uint8_t, N> AsArray() const;

		      std::vector<uint8_t>& AsVector();
		const std::vector<uint8_t>& AsVector() const;

		std::string AsString() const;



	private:
		std::vector<uint8_t> mData;
	};
}



template<typename T>
Strawberry::Standard::IO::DynamicByteBuffer::DynamicByteBuffer(const T* data, size_t len)
	: mData(reinterpret_cast<const uint8_t*>(data), reinterpret_cast<const uint8_t*>(data) + len)
{

}



template<typename T>
Strawberry::Standard::IO::DynamicByteBuffer::DynamicByteBuffer(const T& object)
	: mData(reinterpret_cast<const uint8_t*>(&object), reinterpret_cast<const uint8_t*>(&object) + sizeof(T))
{

}



template <typename T>
void Strawberry::Standard::IO::DynamicByteBuffer::Push(const T* data, size_t count)
{
	for (int i = 0; i < count; i++)
	{
		Push(data[count]);
	}
}



template <typename T>
void Strawberry::Standard::IO::DynamicByteBuffer::Push(const T& data)
{
	auto bytes = reinterpret_cast<const uint8_t*>(&data);
	for (int i = 0; i < sizeof(T); i++)
	{
		mData.push_back(bytes[i]);
	}
}



template<typename T>
void Strawberry::Standard::IO::DynamicByteBuffer::Push(const std::vector<T>& data)
{
	Push(data.data(), data.size());
}



template<size_t N>
std::array<uint8_t, N> Strawberry::Standard::IO::DynamicByteBuffer::AsArray() const
{
	Assert(Size() == N);
	std::array<uint8_t, N> array;
	std::copy(mData.begin(), mData.end(), array.begin());
	return array;
}



template<typename T> requires std::copyable<T> || std::movable<T>
T Strawberry::Standard::IO::DynamicByteBuffer::Into() const
{
	Assert(Size() == sizeof(T));
	T value;
	Into<T>(value);
	return value;
}



template<typename T>
void Strawberry::Standard::IO::DynamicByteBuffer::Into(T& data) const
{
	Assert(Size() == sizeof(T));
	memcpy(reinterpret_cast<void*>(&data), reinterpret_cast<const void*>(Data()), sizeof(T));
}



template<size_t S>
Strawberry::Standard::IO::ByteBuffer<S> Strawberry::Standard::IO::DynamicByteBuffer::AsStatic() const
{
	return ByteBuffer<S>(Data(), std::min(S, Size()));
}
#pragma once



#include <vector>
#include <cstdint>
#include "Standard/Assert.hpp"
#include "Standard/Result.hpp"



namespace Strawberry::Standard::IO
{
	class DynamicByteBuffer
	{
	public:
		// Constructors
		DynamicByteBuffer() = default;
		DynamicByteBuffer(size_t capacity);




		// Pushing Data
		template <typename T>
		void Push(const T* data, size_t count);

		template <typename T>
		void Push(const T& data);

		template <typename T>
		void Push(const std::vector<T>& data);



		// Accessors
		size_t Size() const;

		      uint8_t* Data();
		const uint8_t* Data() const;

		      uint8_t& operator[](size_t i)       { return mData[i]; }
		const uint8_t& operator[](size_t i) const { return mData[i]; }



		// Casting
		template <typename T> requires std::copyable<T> || std::movable<T>
		T IntoType() const;

		template <typename T>
		void IntoType(T& data) const;

		template <size_t N>
		std::array<uint8_t, N> AsArray() const;

		      std::vector<uint8_t>& AsVector();
		const std::vector<uint8_t>& AsVector() const;



	private:
		std::vector<uint8_t> mData;
	};
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
T Strawberry::Standard::IO::DynamicByteBuffer::IntoType() const
{
	Assert(Size() == sizeof(T));
	T value;
	IntoType<T>(value);
	return value;
}



template<typename T>
void Strawberry::Standard::IO::DynamicByteBuffer::IntoType(T& data) const
{
	Assert(Size() == sizeof(T));
	memcpy(reinterpret_cast<void*>(&data), reinterpret_cast<const void*>(Data()), sizeof(T));
}

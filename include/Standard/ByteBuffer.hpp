#pragma once



#include <vector>
#include <cstdint>
#include "Assert.hpp"



namespace Strawberry::Standard
{
	class ByteBuffer
	{
	public:
		template <typename T>
		void Push(const T* data, size_t count);

		template <typename T>
		void Push(const T& data) requires ( std::is_fundamental_v<T> );

		template <typename T>
		void Push(const std::vector<T>& data) requires ( std::is_fundamental_v<T> );



		size_t Size() const;


		      uint8_t* Data();
		const uint8_t* Data() const;



		template <size_t N>
		std::array<uint8_t, N> AsArray() const;


		      std::vector<uint8_t>& AsVector();
		const std::vector<uint8_t>& AsVector() const;



	private:
		std::vector<uint8_t> mBytes;
	};
}



template <typename T>
void Strawberry::Standard::ByteBuffer::Push(const T* data, size_t count)
{
	for (int i = 0; i < count; i++)
	{
		Push(data[count]);
	}
}



template <typename T>
void Strawberry::Standard::ByteBuffer::Push(const T& data) requires(std::is_fundamental_v<T>)
{
	auto bytes = reinterpret_cast<const uint8_t*>(&data);
	for (int i = 0; i < sizeof(T); i++)
	{
		mBytes.push_back(bytes[i]);
	}
}



template<typename T>
void Strawberry::Standard::ByteBuffer::Push(const std::vector<T>& data) requires(std::is_fundamental_v<T>)
{
	Push(data.data(), data.size());
}



template<size_t N>
std::array<uint8_t, N> Strawberry::Standard::ByteBuffer::AsArray() const
{
	Assert(Size() == N);
	std::array<uint8_t, N> array;
	std::copy(mBytes.begin(), mBytes.end(), array.begin());
	return array;
}

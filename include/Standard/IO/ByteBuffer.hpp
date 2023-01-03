#pragma once



#include <cstdlib>
#include <array>



namespace Strawberry::Standard::IO
{
	template <size_t S>
	class ByteBuffer
	{
	public:
		// Constructors
		ByteBuffer() = default;
		ByteBuffer(const uint8_t* data, size_t len);



		size_t Size() const { return S; }

		      uint8_t* Data()       { return mData; }
		const uint8_t* Data() const { return mData; }

		      uint8_t& operator[](size_t i)       { return mData[i]; }
		const uint8_t& operator[](size_t i) const { return mData[i]; }



		template <typename T> requires (sizeof(T) == S) && (std::copyable<T> || std::movable<T>)
		T IntoType();

		template <typename T> requires (sizeof(T) == S)
		void IntoType(T& data);



	private:
		std::array<uint8_t, S> mData;



	};





	template<size_t S>
	template<typename T> requires (sizeof(T) == S) && (std::copyable<T> || std::movable<T>)
	T ByteBuffer<S>::IntoType()
	{
		T value;
		IntoType(value);
		return value;
	}



	template<size_t S>
	template<typename T> requires (sizeof(T) == S)
	void ByteBuffer<S>::IntoType(T& data)
	{
		memcpy(reinterpret_cast<void*>(&data), reinterpret_cast<const void*>(mData.data()), sizeof(T));
	}
}
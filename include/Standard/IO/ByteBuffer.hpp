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



		// Accessors
		size_t Size() const { return S; }

		      uint8_t* Data()       { return mData; }
		const uint8_t* Data() const { return mData; }

		      uint8_t& operator[](size_t i)       { return mData[i]; }
		const uint8_t& operator[](size_t i) const { return mData[i]; }



		// Comparison
		std::strong_ordering operator<=>(const ByteBuffer<S>& rhs) const;
		bool operator==(const ByteBuffer<S>& rhs) const = default;
		bool operator!=(const ByteBuffer<S>& rhs) const = default;
		bool operator >(const ByteBuffer<S>& rhs) const = default;
		bool operator <(const ByteBuffer<S>& rhs) const = default;
		bool operator>=(const ByteBuffer<S>& rhs) const = default;
		bool operator<=(const ByteBuffer<S>& rhs) const = default;



		template <typename T> requires (sizeof(T) == S) && (std::copyable<T> || std::movable<T>)
		T IntoType();

		template <typename T> requires (sizeof(T) == S)
		void IntoType(T& data);



	private:
		std::array<uint8_t, S> mData;



	};





	template<size_t S>
	std::strong_ordering ByteBuffer<S>::operator<=>(const ByteBuffer<S>& rhs) const
	{
		for (int i = 0; i < S; ++i)
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
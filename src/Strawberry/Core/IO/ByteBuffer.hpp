#pragma once


#include "DynamicByteBuffer.hpp"
#include <array>
#include <cstdlib>


namespace Strawberry::Core::IO
{
	template<size_t S>
	class ByteBuffer
	{
	public:
		// Constructors
		ByteBuffer() = default;

		template<typename T>
		explicit ByteBuffer(const T* data, size_t len = S);

		template<typename T> requires (sizeof(T) == S)
		explicit ByteBuffer(const T& object);


		// Accessors
		[[nodiscard]] size_t Size() const
		{
			return S;
		}


		uint8_t* Data()
		{
			return mData.data();
		}


		[[nodiscard]] const uint8_t* Data() const
		{
			return mData.data();
		}


		uint8_t& operator[](size_t i)
		{
			return mData[i];
		}


		const uint8_t& operator[](size_t i) const
		{
			return mData[i];
		}


		// Comparison
		std::strong_ordering operator<=>(const ByteBuffer<S>& rhs) const;
		bool                 operator==(const ByteBuffer<S>& rhs) const = default;
		bool                 operator!=(const ByteBuffer<S>& rhs) const = default;
		bool                 operator>(const ByteBuffer<S>& rhs) const  = default;
		bool                 operator<(const ByteBuffer<S>& rhs) const  = default;
		bool                 operator>=(const ByteBuffer<S>& rhs) const = default;
		bool                 operator<=(const ByteBuffer<S>& rhs) const = default;


		template<typename T> requires ((sizeof(T) == S) && (std::copyable<T> || std::movable<T>))
		T Into();

		template<typename T> requires (sizeof(T) == S)
		void Into(T& data);

		[[nodiscard]] DynamicByteBuffer ToDynamic() const;

	private:
		std::array<uint8_t, S> mData = {0};
	};


	template<size_t S>
	template<typename T>
	ByteBuffer<S>::ByteBuffer(const T* data, size_t len)
		: mData()
	{
		memcpy(mData.data(), reinterpret_cast<const void*>(data), len);
	}


	template<size_t S>
	template<typename T> requires (sizeof(T) == S)
	ByteBuffer<S>::ByteBuffer(const T& object)
		: mData()
	{
		memcpy(mData.data(), reinterpret_cast<const void*>(&object), sizeof(T));
	}


	template<size_t S>
	std::strong_ordering ByteBuffer<S>::operator<=>(const ByteBuffer<S>& rhs) const
	{
		return mData <=> rhs;
	}


	template<size_t S>
	template<typename T> requires ((sizeof(T) == S) && (std::copyable<T> || std::movable<T>))
	T ByteBuffer<S>::Into()
	{
		T value;
		Into(value);
		return value;
	}


	template<size_t S>
	template<typename T> requires (sizeof(T) == S)
	void ByteBuffer<S>::Into(T& data)
	{
		memcpy(reinterpret_cast<void*>(&data), reinterpret_cast<const void*>(mData.data()), sizeof(T));
	}


	template<size_t S>
	DynamicByteBuffer ByteBuffer<S>::ToDynamic() const
	{
		return DynamicByteBuffer(Data(), S);
	}
} // namespace Strawberry::Core::IO

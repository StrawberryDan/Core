#pragma once


#include "Strawberry/Core/IO/Error.hpp"
#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/Types/Result.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include <array>
#include <compare>
#include <concepts>
#include <cstdint>
#include <vector>
#include <filesystem>
#include <tuple>


namespace Strawberry::Core::IO
{
	template<size_t S>
	class ByteBuffer;


	class DynamicByteBuffer
	{
	public:
		// Static Methods
		static Optional<DynamicByteBuffer>                               FromFile(const std::filesystem::path& path);
		static Optional<std::tuple<Math::Vec2u, int, DynamicByteBuffer>> FromImage(const std::filesystem::path& path);
		static DynamicByteBuffer                                         Zeroes(size_t len);
		static DynamicByteBuffer                                         WithCapacity(size_t len);

		template <typename Arg, typename... Args>
		static constexpr DynamicByteBuffer FromObjects(Arg&& arg, Args&&... args)
		{
			DynamicByteBuffer buffer;

			buffer.Push(std::forward<Arg>(arg));
			if constexpr (sizeof...(args) > 0)
			{
				buffer.Push(FromObjects(std::forward<Args>(args)...));
			}

			return buffer;
		}


		// Constructors
		DynamicByteBuffer() = default;
		template<typename T>
		DynamicByteBuffer(const T* data, size_t len);
		DynamicByteBuffer(const std::string& string);


		void Clear();


		// Pushing Data
		template<typename T>
		void Push(const T* data, size_t count);

		template<typename T>
		void Push(const T& data);

		template<typename T>
		void Push(const std::vector<T>& data);


		void Push(const IO::DynamicByteBuffer& bytes)
		{
			Push(bytes.Data(), bytes.Size());
		}


		template<size_t S>
		void Push(const IO::ByteBuffer<S>& bytes)
		{
			Push(bytes.Data(), bytes.Size());
		}


		void Overwrite(size_t offset, const uint8_t* data, size_t len);


		template <size_t S>
		void Overwrite(size_t offset, const ByteBuffer<S>& bytes)
		{
			Overwrite(offset, bytes.Data(), bytes.Size());
		}


		template <typename T>
		void Overwrite(size_t offset, const T& data)
		{
			Overwrite(offset, reinterpret_cast<const uint8_t*>(&data), sizeof(data));
		}


		Result<DynamicByteBuffer, Error> Read(size_t len);
		Result<size_t, Error>            Write(const DynamicByteBuffer& bytes);


		// Accessors
		[[nodiscard]] size_t Size() const;

		uint8_t*                     Data();
		[[nodiscard]] const uint8_t* Data() const;


		uint8_t& operator[](size_t i)
		{
			return mData[i];
		}


		const uint8_t& operator[](size_t i) const
		{
			return mData[i];
		}


		// Iterators
		uint8_t* begin()
		{
			return Data();
		}


		uint8_t* end()
		{
			return Data() + Size();
		}


		[[nodiscard]] const uint8_t* begin() const
		{
			return Data();
		}


		[[nodiscard]] const uint8_t* end() const
		{
			return Data() + Size();
		}


		// Sizing
		void Reserve(size_t len);
		void Resize(size_t len);


		// Comparison
		std::strong_ordering operator<=>(const DynamicByteBuffer& rhs) const = default;


		// Casting
		template<typename T> requires std::copyable<T> || std::movable<T>
		T Into() const;

		template<typename T>
		void Into(T& data) const;

		template<size_t S>
		ByteBuffer<S> AsStatic() const;

		template<size_t N>
		std::array<uint8_t, N> AsArray() const;

		template<typename T = uint8_t>
		std::vector<T> AsVector() const;

		[[nodiscard]] std::string AsString() const;


		[[nodiscard]] std::string AsHexString() const;

	private:
		std::vector<uint8_t> mData;
		size_t               mReadCursor = 0;
	};
} // namespace Strawberry::Core::IO


template<typename T>
Strawberry::Core::IO::DynamicByteBuffer::DynamicByteBuffer(const T* data, size_t len)
	: mData(reinterpret_cast<const uint8_t*>(data), reinterpret_cast<const uint8_t*>(data) + len) {}


template<typename T>
void Strawberry::Core::IO::DynamicByteBuffer::Push(const T* data, size_t count)
{
	for (int i = 0; i < count; i++)
	{
		Push(data[i]);
	}
}


template<typename T>
void Strawberry::Core::IO::DynamicByteBuffer::Push(const T& data)
{
	const auto bytes = reinterpret_cast<const uint8_t*>(&data);
	for (int i = 0; i < sizeof(T); i++)
	{
		mData.push_back(bytes[i]);
	}
}


template<typename T>
void Strawberry::Core::IO::DynamicByteBuffer::Push(const std::vector<T>& data)
{
	Push(data.data(), data.size());
}


template<size_t N>
std::array<uint8_t, N> Strawberry::Core::IO::DynamicByteBuffer::AsArray() const
{
	Assert(Size() == N);
	std::array<uint8_t, N> array;
	std::copy(mData.begin(), mData.end(), array.begin());
	return array;
}


template<typename T> requires std::copyable<T> || std::movable<T>
T Strawberry::Core::IO::DynamicByteBuffer::Into() const
{
	AssertEQ(Size(), sizeof(T));
	T value;
	Into<T>(value);
	return value;
}


template<typename T>
void Strawberry::Core::IO::DynamicByteBuffer::Into(T& data) const
{
	Assert(Size() == sizeof(T));
	memcpy(reinterpret_cast<void*>(&data), reinterpret_cast<const void*>(Data()), sizeof(T));
}


template<size_t S>
Strawberry::Core::IO::ByteBuffer<S> Strawberry::Core::IO::DynamicByteBuffer::AsStatic() const
{
	return ByteBuffer<S>(Data(), std::min(S, Size()));
}


template<typename T>
std::vector<T> Strawberry::Core::IO::DynamicByteBuffer::AsVector() const
{
	Assert(Size() % sizeof(T) == 0);
	std::vector<T> vector(reinterpret_cast<const T*>(Data()), reinterpret_cast<const T*>(Data()) + Size() / sizeof(T));
	return vector;
}


template<>
std::vector<uint8_t> Strawberry::Core::IO::DynamicByteBuffer::AsVector<uint8_t>() const;

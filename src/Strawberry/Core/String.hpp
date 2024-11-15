#pragma once
//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include <concepts>
#include <cstdint>


//======================================================================================================================
//		Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	using Codepoint = uint32_t;


	template<typename Encoding>
	concept StringEncoding = requires(
		Codepoint      codepoint,
		size_t         index,
		const uint8_t* readBytes,
		uint8_t*       writeBytes)
			{
				{ Encoding::GetCodepoint(readBytes, index) } noexcept -> std::same_as<const Codepoint&>;
				{ Encoding::SetCodepoint(writeBytes, index, codepoint) } noexcept -> std::same_as<void>;

				{ Encoding::Next(readBytes) } noexcept -> std::same_as<const uint8_t*>;
				{ Encoding::Prev(readBytes) } noexcept -> std::same_as<const uint8_t*>;
				{ Encoding::Next(writeBytes) } noexcept -> std::same_as<uint8_t*>;
				{ Encoding::Prev(writeBytes) } noexcept -> std::same_as<uint8_t*>;

				{ Encoding::Length(readBytes, index) } noexcept -> std::same_as<size_t>;
			};


	template<StringEncoding Encoding>
	class String;


	namespace Encoding
	{
		class UTF8
		{
		public:
			static unsigned GetCharWidth(const uint8_t* data) noexcept;
			static unsigned GetCharWidth(Codepoint codepoint) noexcept;

			static const Codepoint& GetCodepoint(const uint8_t* data, size_t index) noexcept;
			static       void       SetCodepoint(uint8_t* data, size_t index, Codepoint codepoint) noexcept;

			template <DecaysTo<uint8_t> PTR>
			static PTR* Next(PTR* data) noexcept;

			template <DecaysTo<uint8_t> PTR>
			static PTR* Prev(PTR* data) noexcept;

			static size_t Length(const uint8_t* data, size_t byteLength) noexcept;
		};


		static_assert(StringEncoding<UTF8>);


		// class UTF16
		// {
		// public:
		// 	static const Codepoint& GetCodepoint(const uint8_t* data, size_t index) noexcept;
		// 	static       void       SetCodepoint(uint8_t* data, size_t index, Codepoint codepoint) noexcept;
		//
		// 	static uint8_t* Next(const uint8_t* data) noexcept;
		// 	static uint8_t* Prev(const uint8_t* data) noexcept;
		//
		// 	static size_t Length(const uint8_t* data) noexcept;
		// };
		//
		//
		// static_assert(StringEncoding<UTF16>);


		class UTF32
		{
		public:
			static const Codepoint& GetCodepoint(const uint8_t* data, size_t index) noexcept;
			static       void       SetCodepoint(uint8_t* data, size_t index, Codepoint codepoint) noexcept;

			template <DecaysTo<uint8_t> PTR>
			static PTR* Next(PTR* data) noexcept;

			template <DecaysTo<uint8_t> PTR>
			static PTR* Prev(PTR* data) noexcept;

			static size_t Length(const uint8_t* data, size_t byteLength) noexcept;
		};


		static_assert(StringEncoding<UTF32>);
	}


	template<StringEncoding Encoding>
	class String
	{
	public:
		template <StringEncoding OtherEncoding>
		String(const String<OtherEncoding>& otherEncoding);

		const Codepoint GetCodepoint(size_t index) const noexcept
		{
			Assert(index < Encoding::Length(mData));
			return Encoding::GetCodepoint(mData, index);
		}


		constexpr void SetCodepoint(size_t index, Codepoint codepoint) noexcept
		{
			Assert(index < Encoding::Length(mData));
			Encoding::SetCodepint(mData, index, codepoint);
		}

	private:
		size_t   mSize = 0;
		uint8_t* mData = nullptr;
	};
}



//======================================================================================================================
//		Includes
//----------------------------------------------------------------------------------------------------------------------
#include "String.hpp"
#include "Markers.hpp"
#include "IO/Logging.hpp"

//======================================================================================================================
//		Method Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	unsigned Encoding::UTF8::GetCharWidth(const uint8_t* data) noexcept
	{
		switch (constexpr auto leadingZeros = std::countl_zero(*data))
		{
			case 0:
				return 1;
			case 2:
				return 2;
			case 3:
				return 3;
			case 4:
				return 4;
			default:
				constexpr const std::string& errorType = (data[0] & 0b1100'0000 == 0b1000'0000) ? "Non-leading byte" : "Invalid leading byte";
				Core::Logging::Error("Invalid UTF-8 leading byte! {:x}. Error type: ", data[0], errorType);
				Core::Unreachable();
		}
	}


	unsigned int Encoding::UTF8::GetCharWith(Codepoint codepoint) noexcept
	{
			if constexpr (codepoint <= 0x007F)
			{
				return 1;
			}
			else if constexpr (codepoint <= 0x07FF)
			{
				return 2;
			}
			else if constexpr (codepoint <= 0xFFFF)
			{
				return 3;
			}
			else if constexpr (codepoint <= 0x10FFFF)
			{
				return 4;
			}
			else
			{
				Core::Unreachable();
			}
	}


	const Codepoint& Encoding::UTF8::GetCodepoint(const uint8_t* data, size_t index) noexcept
	{
		for (int i = 0; i < index; ++index)
		{
			data += GetCharWidth(data);
		}

		Codepoint codepoint = [=]()
		{
			switch (GetCharWidth(data))
			{
				case 1:
					return data[0] & 0b0111'1111;
				case 2:
					return (data[1] & 0b0011'1111) | (data[0] & 0b0001'1111) << 6;
				case 3:
					return (data[2] & 0b0011'1111) | (data[1] & 0b0011'1111) << 6 | (data[0] & 0b0000'1111) << 12;
				case 4:
					return (data[3] & 0b0011'1111) | (data[2] & 0b0011'1111) << 6 | (data[1] & 0b0011'1111) << 12 | (data[2] & 0b0000'0111) << 18;
				default:
					Core::Unreachable();
			}
		}();

		return codepoint;
	}


	inline void Encoding::UTF8::SetCodepoint(uint8_t* data, size_t index, Codepoint codepoint) noexcept
	{
		constexpr uint32_t encoded = []()
		{
			if constexpr (codepoint <= 0x007F)
			{
				return data[0] & 0b0111'1111;
			}
			else if constexpr (codepoint <= 0x07FF)
			{
				return 0b1000'0000 | data[1] & 0b0011'1111 | (0b1100'0000 | data[0] & 0b0011'1111) << 8;
			}
			else if constexpr (codepoint <= 0xFFFF)
			{
				return 0b1000'0000 | data[2] & 0b0011'1111 | (0b1100'0000 | data[1] & 0b0011'1111) << 8 | (0b11100000 | data[0] & 0b0000'1111) << 16;
			}
			else if constexpr (codepoint <= 0x10FFFF)
			{
				return 0b1000'0000 | data[3] & 0b0011'1111 | (0b1100'0000 | data[2] & 0b0011'1111) << 8 | (0b10000000 | data[1] & 0b0011'1111) << 16 | (
					0b1111'0000 | data[0] & 0b0000'0111) << 24;
			}
			else
			{
				Core::Unreachable();
			}
		}();
		const_cast<Codepoint&>(GetCodepoint(data, index)) = encoded;
	}


	template<DecaysTo<uint8_t> PTR>
	PTRi* Encoding::UTF8::Next(PTR* data) noexcept
	{
		return data + GetCharWidth(data);
	}


	template<DecaysTo<uint8_t> PTR>
	PTR* Encoding::UTF8::Prev(PTR* data) noexcept
	{
		for (int i = 0; i < 4; i++)
		{
			if (data[-i - 1] & 0b1100'0000 != 0b1100'0000)
			{
				return data - i - 1;
			}
		}
		Core::Unreachable();
	}


	size_t           Encoding::UTF8::Length(const uint8_t* data, size_t bytelength) noexcept
	{
		size_t length = 0;
		for (size_t i = 0; i < bytelength;)
		{
			i += GetCharWidth(data + i);
			++length;
		}
		return length;
	}


	// const Codepoint& Encoding::UTF16::GetCodepoint(const uint8_t* data, size_t index) noexcept {}
	// void             Encoding::UTF16::SetCodepoint(uint8_t* data, size_t index, Codepoint codepoint) noexcept {}
	// uint8_t*         Encoding::UTF16::Next(const uint8_t* data) noexcept {}
	// uint8_t*         Encoding::UTF16::Prev(const uint8_t* data) noexcept {}
	// size_t           Encoding::UTF16::Length(const uint8_t* data) noexcept {}

	inline const Codepoint& Encoding::UTF32::GetCodepoint(const uint8_t* data, size_t index) noexcept
	{
		Assert(index < Length(data));
		return data[index];
	}
	inline void             Encoding::UTF32::SetCodepoint(uint8_t* data, size_t index, Codepoint codepoint) noexcept
	{
		data[index] = codepoint;
	}


	template<DecaysTo<uint8_t> PTR>
	PTR* Encoding::UTF32::Next(PTR* data) noexcept
	{
		return data + 4;
	}


	template<DecaysTo<uint8_t> PTR>
	PTR* Encoding::UTF32::Prev(PTR* data) noexcept
	{
		return data - 1;
	}


	inline size_t Encoding::UTF32::Length(const uint8_t* data, size_t byteLength) noexcept
	{
		return byteLength;
	}
}

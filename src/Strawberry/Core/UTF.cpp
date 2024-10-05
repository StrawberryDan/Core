//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/UTF.hpp"
// Strawberry Core
#include "Strawberry/Core/Markers.hpp"
#include "Strawberry/Core/Math/Math.hpp"
// Standard Library
#include <vector>
#include <bit>
#include <algorithm>
#include <cstdint>


namespace Strawberry::Core
{
	std::u32string ToUTF32(const std::string& utf8, char32_t placeholder)
	{
		std::u32string result;

		int i = 0;
		while (i < utf8.size())
		{
			auto c = ToUTF32(utf8.data() + i).UnwrapOr(placeholder);
			result.push_back(c);

			if (c <= 0x007F) i += 1;
			else if (c <= 0x07FF) i += 2;
			else if (c <= 0xFFFF) i += 3;
			else if (c <= 0x10FFFF) i += 4;
			else Unreachable();
		}

		return result;
	}


	std::string ToUTF8(const std::u32string& utf32, const std::string placeholder)
	{
		std::string result;

		for (char32_t c32: utf32)
		{
			auto utf8 = ToUTF8(c32).UnwrapOr(placeholder);
			for (char8_t c8: utf8)
			{
				result.push_back(c8);
			}
		}

		return result;
	}


	Optional<char32_t> ToUTF32(const char* utf8)
	{
		uint8_t length             = 0;
		uint8_t firstByteCodePoint = 0;
		if ((utf8[0] & 0b10000000) == 0)
		{
			length             = 1;
			firstByteCodePoint = 0b01111111 & utf8[0];
		}
		else if ((utf8[0] & 0b11100000) == 0b11000000)
		{
			length             = 2;
			firstByteCodePoint = 0b00011111 & utf8[0];
		}
		else if ((utf8[0] & 0b11110000) == 0b11100000)
		{
			length             = 3;
			firstByteCodePoint = 0b00001111 & utf8[0];
		}
		else if ((utf8[0] & 0b11111000) == 0b11110000)
		{
			length             = 4;
			firstByteCodePoint = 0b00000111 & utf8[0];
		}
		else
		{
			Unreachable();
		}

		std::vector<uint8_t> remainingBytes;
		remainingBytes.reserve(3);
		for (int i = 1; i < length; i++)
		{
			remainingBytes.push_back(0b00111111 & utf8[i]);
		}
		std::reverse(remainingBytes.begin(), remainingBytes.end());

		uint8_t  shift     = 0;
		char32_t codepoint = 0;
		for (uint8_t byte: remainingBytes)
		{
			char32_t byteAs32(byte);
			codepoint = codepoint | (byte << shift);
			shift += 6;
		}

		codepoint = codepoint | (firstByteCodePoint << shift);

		return codepoint;
	}


	Optional<std::string> ToUTF8(char32_t utf32)
	{
		const auto leadingZeros    = std::countl_zero<uint32_t>(utf32);
		const auto codepointLength = 32 - leadingZeros;
		if (leadingZeros < 11) return {};

		uint8_t utf8Len = 0;
		if (utf32 <= 0x007F) utf8Len = 1;
		else if (utf32 <= 0x07FF) utf8Len = 2;
		else if (utf32 <= 0xFFFF) utf8Len = 3;
		else if (utf32 <= 0x10FFFF) utf8Len = 4;
		else Unreachable();


		std::vector<char8_t> utf8;
		utf8.reserve(utf8Len);

		uint8_t shift = 0;
		for (int i = utf8Len - 1; i > 0; i--)
		{
			utf8.push_back(((utf32 & (0b00111111 << shift)) >> shift) | 0b10000000);
			shift += 6;
		}

		uint8_t finalByte = 0;
		switch (utf8Len)
		{
			case 1:
				finalByte = 0b00000000 | (utf32 >> shift);
				break;
			case 2:
				finalByte = 0b11000000 | (utf32 >> shift);
				break;
			case 3:
				finalByte = 0b11100000 | (utf32 >> shift);
				break;
			case 4:
				finalByte = 0b11110000 | (utf32 >> shift);
			default:
				Unreachable();
		}
		utf8.push_back(finalByte);

		std::reverse(utf8.begin(), utf8.end());
		utf8.shrink_to_fit();
		return std::string(reinterpret_cast<const char*>(utf8.data()), utf8.size());
	}
}

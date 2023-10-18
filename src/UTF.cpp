//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/UTF.hpp"
// Strawberry Core
#include "Strawberry/Core/Markers.hpp"
// Standard Library
#include <vector>


namespace Strawberry::Core
{
	Optional<char32_t> ToUTF32(const char8_t* utf8)
	{
		uint8_t length = 0;
		uint8_t firstByteCodePoint = 0;
		if ((utf8[0] & 0b10000000) == 0)
		{
			length = 1;
			firstByteCodePoint = 0b01111111 & utf8[0];
		}
		else if ((utf8[0] & 0b11100000) == 0b11000000)
		{
			length = 2;
			firstByteCodePoint = 0b00011111 & utf8[0];
		}
		else if ((utf8[0] & 0b11110000) == 0b11100000)
		{
			length = 3;
			firstByteCodePoint = 0b00001111 & utf8[0];
		}
		else if ((utf8[0] & 0b11111000) == 0b11110000)
		{
			length = 4;
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

		uint8_t  shift = 0;
		char32_t codepoint = 0;
		for (uint8_t byte : remainingBytes)
		{
			char32_t byteAs32(byte);
			codepoint = codepoint | (byte << shift);
			shift += 6;
		}

		codepoint = codepoint | (firstByteCodePoint << shift);

		return codepoint;
	}
}

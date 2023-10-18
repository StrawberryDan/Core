#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <string>

namespace Strawberry::Core
{
	std::u32string ToUTF32(const std::u8string& utf8, const char32_t placeholder = '?');

	std::u8string ToUTF8(const std::u32string& utf32, const std::u8string placeholder = u8"?");

	Optional<char32_t> ToUTF32(const char8_t* utf8);

	Optional<std::u8string> ToUTF8(char32_t utf32);
}

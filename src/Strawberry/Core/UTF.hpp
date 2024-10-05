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
	std::u32string ToUTF32(const std::string& utf8, const char32_t placeholder = '?');

	std::string ToUTF8(const std::u32string& utf32, const std::string placeholder = "?");

	Optional<char32_t> ToUTF32(const char* utf8);

	Optional<std::string> ToUTF8(char32_t utf32);
}

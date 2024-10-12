#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <string>
#include <concepts>

namespace Strawberry::Core
{
	std::u32string     ToUTF32(const std::string& utf8, char32_t placeholder = '?');
	Optional<char32_t> ToUTF32(const char* utf8);


	template<std::convertible_to<char> C>
	Optional<char32_t> ToUTF32(const C* c)
	{
		return ToUTF32(static_cast<const char*>(c));
	}


	std::string ToUTF8(const std::u32string& utf32, const std::string& placeholder = "?");
	Optional<std::string> ToUTF8(char32_t utf32);
}

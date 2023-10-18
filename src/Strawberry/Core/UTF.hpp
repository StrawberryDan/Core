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
	Optional<char32_t> ToUTF32(const char8_t* utf8);

	Optional<std::u8string> ToUTF8(char32_t utf32);
}

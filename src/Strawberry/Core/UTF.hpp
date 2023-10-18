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
}

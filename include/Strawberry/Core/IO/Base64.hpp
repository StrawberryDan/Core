#pragma once


#include <cstdint>
#include <string>
#include <vector>


#include "DynamicByteBuffer.hpp"


namespace Strawberry::Core::IO::Base64
{
	std::string Encode(const DynamicByteBuffer& bytes);

	DynamicByteBuffer Decode(std::string encoded);
} // namespace Strawberry::Core::IO::Base64
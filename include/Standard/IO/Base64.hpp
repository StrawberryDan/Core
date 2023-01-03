#pragma once



#include <string>
#include <vector>
#include <cstdint>



#include "DynamicByteBuffer.hpp"





namespace Strawberry::Standard::IO::Base64
{
    std::string Encode(const DynamicByteBuffer& bytes);

    DynamicByteBuffer Decode(std::string encoded);
}
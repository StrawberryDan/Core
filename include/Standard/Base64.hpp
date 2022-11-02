#pragma once



#include <string>
#include <vector>
#include <cstdint>



namespace Strawberry::Standard::Base64
{
    std::string Encode(const std::vector<uint8_t>& bytes);

    std::vector<uint8_t> Decode(std::string encoded);
}
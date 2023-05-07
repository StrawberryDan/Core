#include "Strawberry/Core/IO/Base64.hpp"
#include "Strawberry/Core/Math/Math.hpp"
#include "Strawberry/Core/Markers.hpp"


#include <utility>
#include <map>
#include <iostream>



static const char encodingTable[64] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/',
};



static const std::map<char, uint8_t> decodingTable =
{
    {'A',  0}, {'B',  1}, {'C',  2}, {'D',  3}, {'E',  4}, {'F',  5}, {'G',  6}, {'H',  7},
    {'I',  8}, {'J',  9}, {'K', 10}, {'L', 11}, {'M', 12}, {'N', 13}, {'O', 14}, {'P', 15},
    {'Q', 16}, {'R', 17}, {'S', 18}, {'T', 19}, {'U', 20}, {'V', 21}, {'W', 22}, {'X', 23},
    {'Y', 24}, {'Z', 25}, {'a', 26}, {'b', 27}, {'c', 28}, {'d', 29}, {'e', 30}, {'f', 31},
    {'g', 32}, {'h', 33}, {'i', 34}, {'j', 35}, {'k', 36}, {'l', 37}, {'m', 38}, {'n', 39},
    {'o', 40}, {'p', 41}, {'q', 42}, {'r', 43}, {'s', 44}, {'t', 45}, {'u', 46}, {'v', 47},
    {'w', 48}, {'x', 49}, {'y', 50}, {'z', 51}, {'0', 52}, {'1', 53}, {'2', 54}, {'3', 55},
    {'4', 56}, {'5', 57}, {'6', 58}, {'7', 59}, {'8', 60}, {'9', 61}, {'+', 62}, {'/', 63},
};



std::string Strawberry::Core::IO::Base64::Encode(const Strawberry::Core::IO::DynamicByteBuffer& bytes)
{
	using namespace Math;



    std::string encoded;
    unsigned long encodedSize = RoundUpToNearestMultiple(CeilDiv(8 * bytes.Size(), 6), 3);

    std::vector<uint8_t> fullSegments(bytes.Data(), bytes.Data() + (3 * (bytes.Size() / 3)));
    std::vector<uint8_t> stragglers(bytes.Data() + (3 * (bytes.Size() / 3)), bytes.Data() + bytes.Size());


    // Encode 3 byte segments
    for (int i = 0; i < fullSegments.size(); i += 3)
    {
        uint8_t a = (fullSegments[i + 0] >> 2)                            & 0b00111111;
        uint8_t b = (fullSegments[i + 0] << 4 | fullSegments[i + 1] >> 4) & 0b00111111;
        uint8_t c = (fullSegments[i + 1] << 2 | fullSegments[i + 2] >> 6) & 0b00111111;
        uint8_t d = (fullSegments[i + 2] >> 0)                            & 0b00111111;

        encoded.push_back(encodingTable[a]);
        encoded.push_back(encodingTable[b]);
        encoded.push_back(encodingTable[c]);
        encoded.push_back(encodingTable[d]);
    }


    switch (stragglers.size())
    {
        case 0:
            break;

        case 1:
        {
            uint8_t a = (stragglers[0] & 0b11111100) >> 2;
            uint8_t b = (stragglers[0] & 0b00000011) << 4;
            encoded += encodingTable[a];
            encoded += encodingTable[b];
            break;
        }

        case 2:
        {
            uint8_t a = ((stragglers[0] & 0b11111100) >> 2);
            uint8_t b = ((stragglers[0] & 0b00000011) << 4) | ((stragglers[1] & 0b11110000) >> 4);
            uint8_t c = ((stragglers[1] & 0b00001111) << 2);
            encoded += encodingTable[a];
            encoded += encodingTable[b];
            encoded += encodingTable[c];
            break;
        }

        default:
	        Unreachable();
    }

    while (encoded.size() < encodedSize)
    {
        encoded.push_back('=');
    }

    return encoded;
}



Strawberry::Core::IO::DynamicByteBuffer Strawberry::Core::IO::Base64::Decode(std::string encoded)
{
    // Delete Padding
    while (encoded.ends_with('='))
    {
        encoded.pop_back();
    }

    size_t fullChunks = (4 * (encoded.size() / 4));
    uint_fast8_t stragglers = encoded.size() % 4;
    DynamicByteBuffer compressed;


    std::vector<uint8_t> uncompressed;
    for (char c : encoded)
    {
        uncompressed.push_back(decodingTable.at(c));
    }

    for (int i = 0; i < fullChunks; i += 4)
    {
        uint8_t a = uncompressed[i + 0] << 2 | uncompressed[i + 1] >> 4;
        uint8_t b = uncompressed[i + 1] << 4 | uncompressed[i + 2] >> 2;
        uint8_t c = uncompressed[i + 2] << 6 | uncompressed[i + 3] >> 0;
        compressed.Push<uint8_t>(a); compressed.Push<uint8_t>(b); compressed.Push<uint8_t>(c);
    }


    switch (stragglers)
    {
        case 0:
            break;

        case 1:
        {
            compressed.Push<uint8_t>(uncompressed[fullChunks + 0] << 6);
            break;
        }

        case 2:
        {
            compressed.Push<uint8_t>(uncompressed[fullChunks + 0] << 2 | uncompressed[fullChunks + 1] >> 4);
            break;
        }

        case 3:
        {
            compressed.Push<uint8_t>(uncompressed[fullChunks + 0] << 2 | uncompressed[fullChunks + 1] >> 4);
            compressed.Push<uint8_t>(uncompressed[fullChunks + 1] << 4 | uncompressed[fullChunks + 2] >> 2);
            break;
        }

        default:
	        Unreachable();
    }


    return compressed;
}

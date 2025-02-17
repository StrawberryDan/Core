#include "Strawberry/Core/IO/Base64.hpp"
#include "Strawberry/Core/Math/Math.hpp"

#include <random>


using namespace Strawberry::Core;


void CheckBytes(const IO::DynamicByteBuffer& bytes)
{
    auto               encoded      = IO::Base64::Encode(bytes);
    auto               decoded      = IO::Base64::Decode(encoded);
    unsigned long long expectedSize = Math::RoundUpToMultiple(Math::CeilDiv(8 * bytes.Size(), 6), 3);
    Assert(encoded.size() == expectedSize);
    Assert(decoded.Size() == bytes.Size());
    Assert(decoded == bytes);
}


int main()
{
    const char*           sample = "Many hands make light work.";
    IO::DynamicByteBuffer sampleBytes(reinterpret_cast<const uint8_t*>(sample), strlen(sample));
    CheckBytes(sampleBytes);

    sample      = "light wo";
    sampleBytes = IO::DynamicByteBuffer(reinterpret_cast<const uint8_t*>(sample), strlen(sample));
    CheckBytes(sampleBytes);

    sample      = "light w";
    sampleBytes = IO::DynamicByteBuffer(reinterpret_cast<const uint8_t*>(sample), strlen(sample));
    CheckBytes(sampleBytes);

    std::random_device                          randomDevice;
    std::mt19937                                randgen(randomDevice());
    std::uniform_int_distribution<unsigned int> lengthDistribution(0, 1024);
    std::uniform_int_distribution<uint8_t>      byteDistribution;

    for (int iterations = 0; iterations < 1024; iterations++)
    {
        unsigned int          len = lengthDistribution(randgen);
        IO::DynamicByteBuffer randomBytes;
        for (int i = 0; i < len; ++i)
        {
            randomBytes.Push(byteDistribution(randgen));
        }

        Assert(randomBytes.Size() == len);
        CheckBytes(randomBytes);
    }
}
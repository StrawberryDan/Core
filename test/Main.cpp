#include "Standard/Base64.hpp"


#include <cstring>
#include "Standard/Assert.hpp"
#include <random>



#include "Standard/Math/Math.hpp"



using namespace Strawberry::Standard;



namespace Test
{
	using namespace Math;



	void CheckBytes(const std::vector<uint8_t>& bytes)
	{
		auto encoded = Base64::Encode(bytes);
		auto decoded = Base64::Decode(encoded);
		unsigned long long expectedSize = RoundUpToNearestMultiple(CeilDiv(8 * bytes.size(), 6), 3);
		Assert(encoded.size() == expectedSize);
		Assert(decoded.size() == bytes.size());
		Assert(decoded == bytes);
	}



	void Base64()
	{
		const char* sample = "Many hands make light work.";
		std::vector<uint8_t> sampleBytes(sample, sample + strlen(sample));
		CheckBytes(sampleBytes);

		sample = "light wo";
		sampleBytes = std::vector<uint8_t>(sample, sample + strlen(sample));
		CheckBytes(sampleBytes);

		sample = "light w";
		sampleBytes = std::vector<uint8_t>(sample, sample + strlen(sample));
		CheckBytes(sampleBytes);

		std::random_device randomDevice;
		std::mt19937 randgen(randomDevice());
		std::uniform_int_distribution<unsigned int> lengthDistribution(0, 1024);
		std::uniform_int_distribution<uint8_t> byteDistribution;

		for (int iterations = 0; iterations < 1024; iterations++)
		{
			unsigned int len = lengthDistribution(randgen);
			std::vector<uint8_t> randomBytes;
			for (int i = 0; i < len; ++i)
			{
				randomBytes.push_back(byteDistribution(randgen));
			}

			CheckBytes(randomBytes);
		}
	}
}



int main()
{
	Test::Base64();
}

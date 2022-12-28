#include "Standard/Base64.hpp"


#include <cstring>
#include <random>



#include "Standard/Assert.hpp"
#include "Standard/Math/Math.hpp"
#include "Standard/Iter.hpp"



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



	void Iterators()
	{
		using namespace Strawberry::Standard::Iter;


		std::vector<int> numbers{1, 2, 3, 4, 5};

		STLWrapper wrap(numbers);
		Assert(wrap.Next() == 1);
		Assert(wrap.Next() == 2);
		Assert(wrap.Next() == 3);
		Assert(wrap.Next() == 4);
		Assert(wrap.Next() == 5);

		auto twice = STLWrapper(numbers).Map<int>([](int x) { return 2 * x; });
		Assert(twice.Next() == 2);
		Assert(twice.Next() == 4);
		Assert(twice.Next() == 6);
		Assert(twice.Next() == 8);
		Assert(twice.Next() == 10);

		auto even = STLWrapper(numbers).Filter([](int x) { return x % 2 == 0; });
		Assert(even.Next() == 2);
		Assert(even.Next() == 4);

		auto sixes = STLWrapper(numbers)
				.Map<int>([](int x) { return 3 * x; })
				.Map<int>([](int x) { return 2 * x; });
		Assert(sixes.Next() == 6);
		Assert(sixes.Next() == 12);


		auto twelves = STLWrapper(numbers)
				.Map<int>([](int x) { return 3 * x;} )
				.Map<int>([](int x) { return 4 * x;} );
		auto half_twelves = twelves.Map<int>([](int x) { return x / 2;});
		Assert(half_twelves.Next() == 1 * 6);
		Assert(half_twelves.Next() == 2 * 6);
		Assert(half_twelves.Next() == 3 * 6);
		Assert(half_twelves.Next() == 4 * 6);
		Assert(half_twelves.Next() == 5 * 6);

		auto even_threes = STLWrapper(numbers)
				.Map<int>([](int x) { return 3 * x; })
				.Filter([](int x) { return x % 2 == 0; });
		Assert(even_threes.Next() == 6);
		Assert(even_threes.Next() == 12);


		auto dropped = STLWrapper(numbers).Drop(2);
		Assert(dropped.Next() == 3);
		Assert(dropped.Next() == 4);
		Assert(dropped.Next() == 5);
	}
}



int main()
{
	Test::Base64();
	Test::Iterators();
}

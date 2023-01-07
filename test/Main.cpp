#include "Standard/IO/Base64.hpp"


#include <cstring>
#include <vector>
#include <map>
#include <random>



#include "Standard/Assert.hpp"
#include "Standard/Math/Math.hpp"
#include "Standard/Iter/STLWrapper.hpp"
#include "Standard/Net/Endpoint.hpp"
#include "Standard/Net/Socket/TCPClient.hpp"
#include "Standard/Net/Socket/TLSClient.hpp"



using namespace Strawberry::Standard;



namespace Test
{
	using namespace Math;



	void CheckBytes(const IO::DynamicByteBuffer& bytes)
	{
		auto encoded = IO::Base64::Encode(bytes);
		auto decoded = IO::Base64::Decode(encoded);
		unsigned long long expectedSize = RoundUpToNearestMultiple(CeilDiv(8 * bytes.Size(), 6), 3);
		Assert(encoded.size() == expectedSize);
		Assert(decoded.Size() == bytes.Size());
		Assert(decoded == bytes);
	}



	void Base64()
	{
		const char* sample = "Many hands make light work.";
		IO::DynamicByteBuffer sampleBytes(reinterpret_cast<const uint8_t*>(sample), strlen(sample));
		CheckBytes(sampleBytes);

		sample = "light wo";
		sampleBytes = IO::DynamicByteBuffer(reinterpret_cast<const uint8_t*>(sample), strlen(sample));
		CheckBytes(sampleBytes);

		sample = "light w";
		sampleBytes = IO::DynamicByteBuffer(reinterpret_cast<const uint8_t*>(sample), strlen(sample));
		CheckBytes(sampleBytes);

		std::random_device randomDevice;
		std::mt19937 randgen(randomDevice());
		std::uniform_int_distribution<unsigned int> lengthDistribution(0, 1024);
		std::uniform_int_distribution<uint8_t> byteDistribution;

		for (int iterations = 0; iterations < 1024; iterations++)
		{
			unsigned int len = lengthDistribution(randgen);
			IO::DynamicByteBuffer randomBytes;
			for (int i = 0; i < len; ++i)
			{
				randomBytes.Push(byteDistribution(randgen));
			}

			Assert(randomBytes.Size() == len);
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



		auto fifteens = STLWrapper(numbers).Map<int>([](int x) { return 15 * x; } );
		auto fifteens_vector = fifteens.Collect<std::vector<int>>();
		Assert(fifteens_vector == std::vector<int>{15, 30, 45, 60, 75});

		auto pairs = std::vector<std::pair<int, int>>{{1, 2}, {3, 4}, {5, 6}};
		auto pairs_iter = STLWrapper(pairs);
		auto pairs_map = pairs_iter.Collect<std::map<int, int>>();
		Assert(pairs_map[1] == 2);
		Assert(pairs_map[3] == 4);
		Assert(pairs_map[5] == 6);
	}



	void ParseIP()
	{
		auto ip = Net::IPv4Address::Parse("255.255.255.255").Unwrap();
		Assert(ip.AsString() == "255.255.255.255");
	}



	void DNS()
	{
		auto justnoise = Strawberry::Standard::Net::Endpoint::Resolve("justnoise.net", 80);
		auto google = Strawberry::Standard::Net::Endpoint::Resolve("google.com", 80);
		Assert(google || justnoise);
	}



	void TCP()
	{
		auto google = Strawberry::Standard::Net::Endpoint::Resolve("google.com", 80).Unwrap();
		auto client = Strawberry::Standard::Net::Socket::TCPClient::Connect(google).Unwrap();
	}



	void TLS()
	{
		{
			auto google = Strawberry::Standard::Net::Endpoint::Resolve("google.com", 443).Unwrap();
			auto client = Strawberry::Standard::Net::Socket::TLSClient::Connect(google).Unwrap();
		}
		{
			auto google = Strawberry::Standard::Net::Endpoint::Resolve("google.com", 443).Unwrap();
			auto client = Strawberry::Standard::Net::Socket::TLSClient::Connect(google).Unwrap();
		}
	}
}



int main()
{
	Test::Base64();
	Test::Iterators();
	Test::ParseIP();
	Test::DNS();
	Test::TCP();
	Test::TLS();
}

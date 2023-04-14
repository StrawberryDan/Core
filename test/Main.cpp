#include "Core/IO/Base64.hpp"


#include <cstring>
#include <vector>
#include <map>
#include <random>



#include "Core/Assert.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Net/Endpoint.hpp"
#include "Core/Net/Socket/TCPClient.hpp"
#include "Core/Net/Socket/TLSClient.hpp"
#include "Core/Collection/CircularBuffer.hpp"
#include "Core/Net/HTTP/Client.hpp"



using namespace Strawberry::Core;



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



	void ParseIP()
	{
		auto ip = Net::IPv4Address::Parse("255.255.255.255").Unwrap();
		Assert(ip.AsString() == "255.255.255.255");
	}



	void DNS()
	{
		auto justnoise = Strawberry::Core::Net::Endpoint::Resolve("justnoise.net", 80);
		auto google = Strawberry::Core::Net::Endpoint::Resolve("google.com", 80);
		Assert(google || justnoise);
	}



	void TCP()
	{
		auto google = Strawberry::Core::Net::Endpoint::Resolve("google.com", 80).Unwrap();
		auto client = Strawberry::Core::Net::Socket::TCPClient::Connect(google).Unwrap();
	}



	void TLS()
	{
		{
			auto google = Strawberry::Core::Net::Endpoint::Resolve("google.com", 443).Unwrap();
			auto client = Strawberry::Core::Net::Socket::TLSClient::Connect(google).Unwrap();
		}
		{
			auto google = Strawberry::Core::Net::Endpoint::Resolve("google.com", 443).Unwrap();
			auto client = Strawberry::Core::Net::Socket::TLSClient::Connect(google).Unwrap();
		}
	}



	void HTTP()
	{
		{
			auto http = Strawberry::Core::Net::HTTP::HTTPSClient("google.com");
			Strawberry::Core::Net::HTTP::Request request(Strawberry::Core::Net::HTTP::Verb::GET, "/");
			http.SendRequest(request);
			auto response = http.Receive();
		}

		{
			auto http = Strawberry::Core::Net::HTTP::HTTPSClient("api.agify.io");
			Strawberry::Core::Net::HTTP::Request request(Strawberry::Core::Net::HTTP::Verb::GET, "/?name=dan");
			http.SendRequest(request);
			auto response = http.Receive();
		}
	}
}



int main()
{
	Test::Base64();
	// Test::Iterators();
	Test::ParseIP();
	Test::DNS();
	Test::TCP();
	Test::TLS();
	Test::HTTP();
}

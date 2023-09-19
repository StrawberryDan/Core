//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Standard Library
#include <cstring>
#include <random>
#include <vector>
// Core
#include "Strawberry/Core/IO/Base64.hpp"
#include "Strawberry/Core/IO/ChannelBroadcaster.hpp"
#include "Strawberry/Core/IO/ChannelReceiver.hpp"
#include "Strawberry/Core/Math/Math.hpp"
#include "Strawberry/Core/Math/Periodic.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Net/Endpoint.hpp"
#include "Strawberry/Core/Net/HTTP/Client.hpp"
#include "Strawberry/Core/Net/Socket/API.hpp"
#include "Strawberry/Core/Net/Socket/TCPClient.hpp"
#include "Strawberry/Core/Net/Socket/TLSClient.hpp"
#include "Strawberry/Core/Util/Assert.hpp"
#include "Strawberry/Core/Util/MaybeUninitialised.hpp"


using namespace Strawberry::Core;

namespace Test
{
	using namespace Math;

	void CheckBytes(const IO::DynamicByteBuffer& bytes)
	{
		auto               encoded      = IO::Base64::Encode(bytes);
		auto               decoded      = IO::Base64::Decode(encoded);
		unsigned long long expectedSize = RoundUpToNearestMultiple(CeilDiv(8 * bytes.Size(), 6), 3);
		Assert(encoded.size() == expectedSize);
		Assert(decoded.Size() == bytes.Size());
		Assert(decoded == bytes);
	}

	void Base64()
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
			for (int i = 0; i < len; ++i) { randomBytes.Push(byteDistribution(randgen)); }

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
		auto google    = Strawberry::Core::Net::Endpoint::Resolve("google.com", 80);
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
			auto                                 http = Strawberry::Core::Net::HTTP::HTTPSClient("google.com");
			Strawberry::Core::Net::HTTP::Request request(Strawberry::Core::Net::HTTP::Verb::GET, "/");
			http.SendRequest(request);
			auto response = http.Receive();
		}

		{
			auto                                 http = Strawberry::Core::Net::HTTP::HTTPSClient("api.agify.io");
			Strawberry::Core::Net::HTTP::Request request(Strawberry::Core::Net::HTTP::Verb::GET, "/?name=dan");
			http.SendRequest(request);
			auto response = http.Receive();
		}
	}

	void PeriodicNumbers()
	{
		using namespace Strawberry::Core::Math;

		Periodic<unsigned int, 10> unsignedInt = 5;
		Assert(unsignedInt + 10 == 5);
		Assert(unsignedInt + 14 == 9);
		Assert(unsignedInt - 6 == 9);
		Assert(unsignedInt - 10 == 5);
		Assert(unsignedInt - 15 == 0);
		Assert(unsignedInt * 2 == 0);
		Assert(unsignedInt * 3 == 5);
		Assert(unsignedInt * 4 == 0);
		Assert(unsignedInt / 2 == 2);

		DynamicPeriodic<unsigned int> dynamicUnsignedInt(10, 5);
		Assert(dynamicUnsignedInt + 10 == 5);
		Assert(dynamicUnsignedInt + 14 == 9);
		Assert(dynamicUnsignedInt - 6 == 9);
		Assert(dynamicUnsignedInt - 10 == 5);
		Assert(dynamicUnsignedInt - 15 == 0);
		Assert(dynamicUnsignedInt * 2 == 0);
		Assert(dynamicUnsignedInt * 3 == 5);
		Assert(dynamicUnsignedInt * 4 == 0);
		Assert(dynamicUnsignedInt / 2 == 2);

		DynamicPeriodic<double> dynamicDouble(10, 5);
		Assert(dynamicDouble + 10 == 5);
		Assert(dynamicDouble + 14 == 9);
		Assert(dynamicDouble - 6 == 9);
		Assert(dynamicDouble - 10 == 5);
		Assert(dynamicDouble - 15 == 0);
		Assert(dynamicDouble * 2 == 0);
		Assert(dynamicDouble * 3 == 5);
		Assert(dynamicDouble * 4 == 0);
		Assert(dynamicDouble / 2 == 2.5);
	}

	class UninitialisedTester
	{
	public:
		UninitialisedTester() { numConstructed++; }

		~UninitialisedTester() { numConstructed--; }

	public:
		static int numConstructed;
	};

	inline int UninitialisedTester::numConstructed = 0;

	void Uninitialised()
	{
		std::vector<Strawberry::Core::MaybeUninitialised<UninitialisedTester>> data(5);

		Strawberry::Core::Assert(UninitialisedTester::numConstructed == 0);
		data[0].Construct();
		Strawberry::Core::Assert(UninitialisedTester::numConstructed == 1);
		data[1].Construct();
		data[2].Construct();
		Strawberry::Core::Assert(UninitialisedTester::numConstructed == 3);
		data[1].Destruct();
		Strawberry::Core::Assert(UninitialisedTester::numConstructed == 2);
	}

	void ChannelBroadcasterReceiver()
	{
		struct A {};

		struct B {};

		struct C {};

		struct D {};

		struct R1 : public Strawberry::Core::IO::ChannelReceiver<A, B> {
			void Receive(A a) {}

			void Receive(B b) {}
		};

		struct R2 : public Strawberry::Core::IO::ChannelReceiver<A, B, C, D> {
			void Receive(A a) {}

			void Receive(B b) {}

			void Receive(C c) {}

			void Receive(D d) {}
		};

		Strawberry::Core::IO::ChannelBroadcaster<A, B, C> b1;
		R1                                                r1;
		R2                                                r2;

		b1.Register(&r1);
		b1.Register(&r2);
	}

	void Vectors() {
		Vector a(1, 2, 3);
		Vector b(4, 5, 6);

		auto c = a.Dot(b);
		auto d = a.Cross(b);

		double e = d.Magnitude();
		float f = d.Magnitude();
	}
} // namespace Test

int main()
{
	Strawberry::Core::Net::Socket::API::Initialise();
	Test::Base64();
	Test::ParseIP();
	Test::DNS();
	Test::TCP();
	Test::TLS();
	Test::HTTP();
	Test::PeriodicNumbers();
	Test::Uninitialised();
	Test::ChannelBroadcasterReceiver();
	Test::Vectors();
	Strawberry::Core::Net::Socket::API::Terminate();
}

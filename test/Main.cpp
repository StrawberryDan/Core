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
#include "Strawberry/Core/Math/Matrix.hpp"
#include "Strawberry/Core/Math/Periodic.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/Types/Uninitialised.hpp"
#include "Strawberry/Core/UTF.hpp"
#include "Strawberry/Core/Math/Clamped.hpp"
#include "Strawberry/Core/Math/AABB.hpp"
#include "Strawberry/Core/Types/TypeSet.hpp"


namespace Test
{
	using namespace Strawberry::Core;
	using namespace Math;


	void TestTypeSets()
	{
		struct A {};
		struct B {};
		struct C {};
		struct D {};


		static_assert(std::same_as<TypeSet<A, B, C, D>::Into<Variant>, Variant<A, B, C, D>>);


		static_assert(
			TypeSet<A, B>::Union<C>::Equals<
			TypeSet<A, B, C>
		>);

		static_assert(
			TypeSet<A, B>::Union<C>::Union<D>::Equals<
			TypeSet<A, B, C, D>
		>);

		static_assert(
			TypeSet<A, B>::Union<TypeSet<C, D>>::Equals<
			TypeSet<A, B, C, D>>);


		static_assert(
			TypeSet<A, B, C, D>::Intersection<A>::Equals<
				TypeSet<A>>);

		static_assert(
			TypeSet<A, B, C, D>::Intersection<A, B>::Equals<
				TypeSet<A, B>>);

		static_assert(
			TypeSet<C, D>::Intersection<TypeSet<A, C>>::Equals<
				TypeSet<C>>);
	}


	void CheckBytes(const IO::DynamicByteBuffer& bytes)
	{
		auto               encoded      = IO::Base64::Encode(bytes);
		auto               decoded      = IO::Base64::Decode(encoded);
		unsigned long long expectedSize = RoundUpToMultiple(CeilDiv(8 * bytes.Size(), 6), 3);
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
			for (int i = 0; i < len; ++i)
			{
				randomBytes.Push(byteDistribution(randgen));
			}

			Assert(randomBytes.Size() == len);
			CheckBytes(randomBytes);
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


	void TestUninitialised()
	{
		static int numConstructed = 0;
		numConstructed = 0;

		struct UninitialisedTester
		{
			UninitialisedTester()
			{
				numConstructed++;
			}


			~UninitialisedTester()
			{
				numConstructed--;
			}
		};

		using T = Uninitialised<UninitialisedTester>;

		std::vector<T> data(5);

		AssertEQ(numConstructed, 0);
		data[0].Construct();
		AssertEQ(numConstructed, 1);
		data[1].Construct();
		data[2].Construct();
		AssertEQ(numConstructed, 3);
		data[1].Destruct();
		AssertEQ(numConstructed, 2);
		data[0].Destruct();
		data[2].Destruct();
		AssertEQ(numConstructed, 0);
	}


	void ChannelBroadcasterReceiver()
	{
		struct A {};

		struct B {};

		struct C {};

		struct D {};

		struct R1 : public Strawberry::Core::IO::ChannelReceiver<A, B>
		{
			void Receive(const A& a) override {}

			void Receive(const B& b)  override {}
		};

		struct R2 : public Strawberry::Core::IO::ChannelReceiver<A, B, C, D>
		{
			void Receive(const A& a) override {}

			void Receive(const B& b) override {}

			void Receive(const C& c) override {}

			void Receive(const D& d) override {}
		};

		Strawberry::Core::IO::ChannelBroadcaster<A, B, C> b1;
		R1                                                r1;
		R2                                                r2;

		b1.Register(r1);
		b1.Register(r2);
	}


	void Vectors()
	{
		Vector a(1, 2, 3);
		Vector b(4, 5, 6);

		auto c = a.Dot(b);
		auto d = a.Cross(b);

		double e = d.Magnitude();
		float  f = d.Magnitude();

		auto& [x, y, z] = a;
		x += 1;

		Strawberry::Core::Assert(a == Vector(2, 2, 3));
	}


	void Matrices()
	{
		Matrix<int, 2, 2> m(1,
		                    2,
		                    3,
		                    4);
		Assert(m[0][0] == 1);
		Assert(m[1][0] == 3);
		Assert(m[0][1] == 2);
		Assert(m[1][1] == 4);
	}


	void UTF()
	{
		const char*    a   = "£";
		char32_t       a32 = ToUTF32(a).Unwrap();
		std::u32string a32str{a32};
		Assert(a32str == U"£");
		Assert(ToUTF8(a32).Unwrap() == a);

		std::u32string b  = U"兎田ぺこら";
		std::string    b8 = ToUTF8(b);
		Assert(ToUTF32(b8) == b);
	}


	void ClampedNumbers()
	{
		// Test clamped number addition
		{
			Clamped<int> a(0, 10, 5);
			Clamped<int> b(0, 10, 5);
			Clamped<int> c = a + b;
			Assert(c == 10);
		}

		// Test addition which overflows bounds
		{
			Clamped<int> a(0, 10, 5);
			Clamped<int> b(0, 10, 6);
			Clamped<int> c = a + b;
			Assert(c == 10);
		}

		// Test subtraction
		{
			Clamped<int> a(0, 10, 5);
			Clamped<int> b(0, 10, 5);
			Clamped<int> c = a - b;
			Assert(c == 0);
		}

		// Test overflowing subtraction
		{
			Clamped<int> a(0, 10, 5);
			Clamped<int> b(0, 10, 6);
			Clamped<int> c = a - b;
			Assert(c == 0);
		}


		// Test overflowing multiplication
		{
			Clamped<int> a(0, 10, 5);
			Clamped<int> b(0, 10, 3);
			Clamped<int> c = a * b;
			Assert(c == 10);
		}

		// Test underflowing division
		{
			Clamped<int> a(3, 10, 5);
			Clamped<int> b(3, 10, 3);
			Clamped<int> c = a / b;
			Assert(c == 3);
		}
	}


	void StaticClampedNumbers()
	{
		// Test clamped number addition
		{
			StaticClamped<int, 0, 10> a(5);
			StaticClamped<int, 0, 10> b(5);
			StaticClamped<int, 0, 10> c = a + b;
			Assert(c == 10);
		}

		// Test addition which overflows bounds
		{
			StaticClamped<int, 0, 10> a(5);
			StaticClamped<int, 0, 10> b(6);
			StaticClamped<int, 0, 10> c = a + b;
			Assert(c == 10);
		}

		// Test subtraction
		{
			StaticClamped<int, 0, 10> a(5);
			StaticClamped<int, 0, 10> b(5);
			StaticClamped<int, 0, 10> c = a - b;
			Assert(c == 0);
		}

		// Test overflowing subtraction
		{
			StaticClamped<int, 0, 10> a(5);
			StaticClamped<int, 0, 10> b(6);
			StaticClamped<int, 0, 10> c = a - b;
			Assert(c == 0);
		}


		// Test overflowing multiplication
		{
			StaticClamped<int, 0, 10> a(5);
			StaticClamped<int, 0, 10> b(3);
			StaticClamped<int, 0, 10> c = a * b;
			Assert(c == 10);
		}

		// Test underflowing division
		{
			StaticClamped<int, 3, 10> a(5);
			StaticClamped<int, 3, 10> b(3);
			StaticClamped<int, 3, 10> c = a / b;
			Assert(c == 3);
		}
	}


	void Variants()
	{
		using V1 = Variant<std::string, int, uint8_t>;
		V1 a = std::string();
		V1 b = int(2);
		V1 c = uint8_t(2);

		Assert(a.IsType<std::string>());
		Assert(b.IsType<int>());
		Assert(c.IsType<uint8_t>());

		Assert(a == "");
		Assert(b == 2);
		Assert(c == 2);

		Assert(b == c);
		Assert(c == b);
		Assert(a != b);
	}
} // namespace Test

int main()
{
	Test::TestTypeSets();
	Test::Base64();
	Test::PeriodicNumbers();
	Test::TestUninitialised();
	Test::ChannelBroadcasterReceiver();
	Test::Vectors();
	Test::Matrices();
	Test::UTF();
	Test::ClampedNumbers();
	Test::StaticClampedNumbers();
	Test::Variants();
}

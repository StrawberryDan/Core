#pragma once



#include <vector>
#include <string>
#include <cstdint>



#include "Standard/Option.hpp"
#include "Standard/IO/ByteBuffer.hpp"
#include "Standard/IO/DynamicByteBuffer.hpp"



namespace Strawberry::Standard::Net
{
	class IPv4Address
	{
	public:
		// Static constructors
		static Option<IPv4Address> Parse(const std::string& data);



	public:
		// Constructors
		IPv4Address(const IO::ByteBuffer<4> bytes)
			: mData(bytes)
		{}



		// Casting
		IO::ByteBuffer<4> AsBytes()  const;
		std::string       AsString() const;

	private:
		IO::ByteBuffer<4> mData;
	};



	class IPv6Address
	{
	public:
		static Option<IPv6Address> Parse(const std::string& string);



	public:
		IPv6Address()
			: mData()
		{}

		IPv6Address(const IO::DynamicByteBuffer& bytes)
			: mData(bytes.AsStatic<16>())
		{}



		// Casting
		const IO::ByteBuffer<16>& AsBytes() const;
		std::string              AsString() const;

	private:
		IO::ByteBuffer<16> mData;
	};



	using IPAddress = std::variant<IPv4Address, IPv6Address>;
}
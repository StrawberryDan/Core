#pragma once



#include <vector>
#include <string>
#include <cstdint>



#include "Strawberry/Core/Util/Option.hpp"
#include "Strawberry/Core/IO/ByteBuffer.hpp"
#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"



namespace Strawberry::Core::Net
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

		IPv4Address(const IO::DynamicByteBuffer& bytes)
				: mData(bytes.AsStatic<4>())
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
		IPv6Address(const IO::ByteBuffer<16>& bytes)
				: mData(bytes)
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



	class IPAddress
	{
	public:
		IPAddress(IPv4Address address);
		IPAddress(IPv6Address address);



		// Checking
		inline bool IsIPv4() const { return std::holds_alternative<IPv4Address>(mPayload); }
		inline bool IsIPv6() const { return std::holds_alternative<IPv6Address>(mPayload); }



		// Casting
		Option<IPv4Address> AsIPv4() const;
		Option<IPv6Address> AsIPv6() const;
		const IO::DynamicByteBuffer AsBytes() const;
		std::string                 AsString() const;



	private:
		std::variant<IPv4Address, IPv6Address> mPayload;
	};
}
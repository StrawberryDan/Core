#include "Standard/Net/Address.hpp"



#include "Standard/Assert.hpp"
#include <numeric>



#if defined(_WIN32)

#elif defined(__APPLE__) || defined(__linux__)
#include <arpa/inet.h>
#include <netdb.h>
#endif



namespace Strawberry::Standard::Net
{
	Option<IPv4Address> IPv4Address::Parse(const std::string& data)
	{
		uint8_t buffer[sizeof(in_addr)] = {0};
		auto result = inet_pton(AF_INET, data.data(), buffer);
		if (result == 1)
		{
			IO::ByteBuffer<4> bytes(buffer, 4);
			return IPv4Address(bytes);
		}
		else
		{
			return {};
		}
	}



	IO::ByteBuffer<4> IPv4Address::AsBytes()
	{
		return mData;
	}



	std::string IPv4Address::AsString()
	{
		char buffer[INET_ADDRSTRLEN] = {0};
		auto result = inet_ntop(AF_INET, mData.Data(), buffer, INET_ADDRSTRLEN);
		Assert(result != nullptr);
		return {buffer};
	}



	Option<IPv6Address> IPv6Address::Parse(const std::string& string)
	{
		uint8_t buffer[sizeof(in6_addr)] = {0};
		auto result = inet_pton(AF_INET6, string.data(), buffer);
		if (result == 1)
		{
			IO::ByteBuffer<16> bytes(buffer, 16);
			return IPv6Address(bytes);
		}
		else
		{
			return {};
		}
	}



	const IO::ByteBuffer<16>& IPv6Address::AsBytes() const
	{
		return mData;
	}



	std::string IPv6Address::AsString() const
	{
		char buffer[INET6_ADDRSTRLEN] = {0};
		auto result = inet_ntop(AF_INET6, mData.Data(), buffer, INET6_ADDRSTRLEN);
		Assert(result != nullptr);
		return {buffer};
	}
}
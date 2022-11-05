#pragma once



#include <vector>
#include <string>
#include <cstdint>



#include "Standard/Option.hpp"



namespace Strawberry::Standard::Net
{
	class IPAddress
	{
	public:
		virtual std::vector<uint8_t> AsBytes() = 0;
		virtual std::string AsString() = 0;
	};



	class IPv4Address
		: public IPAddress
	{
	public:
		static Option<IPv4Address> Parse(const std::string& data);

		std::vector<uint8_t> AsBytes() override;
		std::string AsString() override;

	private:
		uint8_t mData[4];
	};
}
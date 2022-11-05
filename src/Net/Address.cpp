#include "Standard/Net/Address.hpp"



#include <regex>
#include <sstream>
#include <numeric>
#include "Standard/Assert.hpp"



namespace Strawberry::Standard::Net
{
	Option<IPv4Address> IPv4Address::Parse(const std::string& data)
	{
		std::regex pattern(R"(^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$)");
		std::smatch match;
		if (!std::regex_match(data, match, pattern))
		{
			return {};
		}
		Assert(match.size() == 5);


		std::vector<std::string> bytesStrs;
		for (int i = 1; i < match.size(); i++)
		{
			bytesStrs.push_back(match[i]);
		}


		std::vector<uint8_t> bytes;
		for (auto str : bytesStrs)
		{
			auto val = std::stoul(str);
			if (val > 255)
			{
				return {};
			}

			bytes.push_back(static_cast<uint8_t>(val));
		}

		IPv4Address addr;
		std::copy(bytes.begin(), bytes.end(), addr.mData);
		return addr;
	}



	std::vector<uint8_t> IPv4Address::AsBytes()
	{
		return {mData, mData + 4};
	}



	std::string IPv4Address::AsString()
	{
		return std::reduce(
				mData + 1, mData + 4, std::to_string(mData[0]),
		        [](auto a, auto b) { return a + "." + std::to_string(b); } );
	}
}
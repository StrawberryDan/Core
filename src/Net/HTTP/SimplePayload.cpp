#include "Standard/Net/HTTP/SimplePayload.hpp"



#include <iostream>
#include <utility>



namespace Strawberry::Standard::Net::HTTP
{
	SimplePayload::SimplePayload(std::vector<uint8_t>  bytes)
	    : mData(std::move(bytes))
	{

	}



	void SimplePayload::Write(const uint8_t* data, size_t len)
	{
	    mData.insert(mData.end(), data, data + len);
	}



	Option<nlohmann::json> SimplePayload::AsJSON()
	{
	    std::string asString(mData.begin(), mData.end());

	    nlohmann::json json;
	    try
	    {
	        json = nlohmann::json::parse(asString.begin(), asString.end());
	    }
	    catch (std::exception& e)
	    {
	        std::cout << e.what() << std::endl;
	        return {};
	    }

	    if (json.is_discarded())
	    {
	        return {};
	    }
	    else
	    {
	        return json;
	    }
	}
}
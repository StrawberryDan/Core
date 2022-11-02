#include <utility>

#include "Standard/Net/HTTP/Response.hpp"


namespace Strawberry::Standard::Net::HTTP
{
	Response::Response(Version mVersion, unsigned int mStatus, std::string mStatusText)
	    : mVersion(mVersion)
	    , mStatus(mStatus)
	    , mStatusText(std::move(mStatusText))
	{

	}
}

#include <utility>

#include "Strawberry/Core/Net/HTTP/Response.hpp"


namespace Strawberry::Core::Net::HTTP
{
	Response::Response(Version mVersion, unsigned int mStatus, std::string mStatusText)
		: mVersion(mVersion)
		, mStatus(mStatus)
		, mStatusText(std::move(mStatusText))
	{}
} // namespace Strawberry::Core::Net::HTTP
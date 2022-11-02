#include <utility>

#include "Standard/Net/HTTP/Request.hpp"



namespace Strawberry::Standard::Net::HTTP
{
	Request::Request(Verb verb, std::string uri, Version version)
	    : mVerb(verb)
	    , mURI(std::move(uri))
	    , mVersion(version)
	{

	}
}

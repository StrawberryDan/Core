#include "Standard/Net/Endpoint.hpp"



#if defined(__APPLE__) || defined(__linux__)
#include <netdb.h>
#endif



namespace Strawberry::Standard::Net
{
	Result<Endpoint, Error> Endpoint::Resolve(const std::string& hostname, uint16_t port)
	{
		addrinfo hints { .ai_flags = AI_ALL | AI_ADDRCONFIG };
		addrinfo* peer = nullptr;
		auto result = getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hints, &peer);

		if (result != 0)
		{
			return Error::DNSResolutionFailure;
		}

		addrinfo* cursor = peer;
		while (cursor != nullptr)
		{
			if (cursor->ai_family == AF_INET)
			{
				auto ipData = reinterpret_cast<sockaddr_in*>(cursor->ai_addr);
				IPv4Address addr(ipData->sin_addr.s_addr);
				return Endpoint(addr, port);
			}
			else if (cursor->ai_family == AF_INET6)
			{
				auto ipData = reinterpret_cast<sockaddr_in6*>(cursor->ai_addr);
				IPv6Address addr({&ipData->sin6_addr, 16});
				return Endpoint(addr, port);
			}

			cursor = cursor->ai_next;
		}

		return Error::DNSResolutionFailure;
	}



	Endpoint::Endpoint(IPAddress address, uint16_t port)
		: mAddress(address)
		, mPort(port)
	{}
}
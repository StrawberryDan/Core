#include "Standard/Net/Endpoint.hpp"



#include "Standard/Net/Socket/API.hpp"



#if defined(__APPLE__) || defined(__linux__)
#include <netdb.h>
#else defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif



namespace Strawberry::Standard::Net
{
	Result<Endpoint, Error> Endpoint::Resolve(const std::string& hostname, uint16_t port)
	{
		Socket::API::Initialise();


		addrinfo hints { .ai_flags = AI_ALL | AI_ADDRCONFIG };
		addrinfo* peer = nullptr;
		auto dnsResult = getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hints, &peer);
		if (dnsResult != 0)
		{
			return Error::DNSResolution;
		}

		Option<Endpoint> result;
		addrinfo* cursor = peer;
		while (cursor != nullptr)
		{
			if (cursor->ai_family == AF_INET)
			{
				auto ipData = reinterpret_cast<sockaddr_in*>(cursor->ai_addr);
				IPv4Address addr(ipData->sin_addr.s_addr);
				result = Endpoint(addr, port);
			}
			else if (cursor->ai_family == AF_INET6)
			{
				auto ipData = reinterpret_cast<sockaddr_in6*>(cursor->ai_addr);
				IPv6Address addr({&ipData->sin6_addr, 16});
				result = Endpoint(addr, port);
			}

			cursor = cursor->ai_next;
		}

		if (result)
		{
			result->mHostName = hostname;
			return *result;
		}
		else
		{
			return Error::DNSResolution;
		}
	}



	Endpoint::Endpoint(IPAddress address, uint16_t port)
		: mAddress(address)
		, mPort(port)
	{}
}
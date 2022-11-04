#include "Standard/Net/HTTP/Client.hpp"



namespace Strawberry::Standard::Net::HTTP
{
	HTTPClient::HTTPClient(const std::string& hostname, uint16_t port)
			: ClientImpl<Socket::TCPClient>(hostname, port)
	{

	};



	HTTPSClient::HTTPSClient(const std::string& hostname, uint16_t port)
			: ClientImpl<Socket::TLSClient>(hostname, port)
	{

	}
}
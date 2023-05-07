#include "Strawberry/Core/Net/Socket/SocketAPI.hpp"



#include <memory>
#include "Strawberry/Core/Assert.hpp"



#if defined(_WIN32)
#include <winsock2.h>
#endif



namespace Strawberry::Core::Net::Socket
{
	SocketAPI::~SocketAPI()
	{
		std::cout << "Terminating Socket API" << std::endl;
#if defined(_WIN32)
		WSACleanup();
#endif
	}



	SocketAPI::SocketAPI()
	{
		std::cout << "Initialising Socket API" << std::endl;
#if defined(_WIN32)
		WSAData wsaData;
		auto err = WSAStartup(MAKEWORD(2, 2), &wsaData);
		Assert(err == 0);
#endif
	}
}
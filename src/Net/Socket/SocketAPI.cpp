#include "Strawberry/Core/Net/Socket/SocketAPI.hpp"



#include "Strawberry/Core/Log.hpp"



#if defined(_WIN32)
#include <winsock2.h>
#endif



namespace Strawberry::Core::Net::Socket
{
	SocketAPI::~SocketAPI()
	{
		Logging::Info("Terminating Socket API");
#if defined(_WIN32)
		WSACleanup();
#endif
	}



	SocketAPI::SocketAPI()
	{
		Logging::Info("Initialising Socket API");
#if defined(_WIN32)
		WSAData wsaData;
		auto err = WSAStartup(MAKEWORD(2, 2), &wsaData);
		Assert(err == 0);
#endif
	}
}
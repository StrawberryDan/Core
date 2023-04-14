#include "Core/Net/Socket/SocketAPI.hpp"



#include <memory>
#include "Core/Assert.hpp"



#if defined(_WIN32)
#include <winsock2.h>
#endif



namespace Strawberry::Core::Net
{
	static std::unique_ptr<SocketAPI> sInstance = nullptr;



	void SocketAPI::Initialise()
	{
		if (!sInstance)
		{
			sInstance = std::unique_ptr<SocketAPI>(new SocketAPI());
		}
	}



	SocketAPI::~SocketAPI()
	{
#if defined(_WIN32)
		WSACleanup();
#endif
	}



	SocketAPI::SocketAPI()
	{
#if defined(_WIN32)
		WSAData wsaData;
		auto err = WSAStartup(MAKEWORD(2, 2), &wsaData);
		Assert(err == 0);
#endif
	}
}
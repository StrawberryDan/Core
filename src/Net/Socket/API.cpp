#include "Core/Net/Socket/API.hpp"



#include <memory>
#include "Core/Assert.hpp"



#if defined(_WIN32)
#include <winsock2.h>
#endif



namespace Strawberry::Core::Net::Socket
{
	static std::unique_ptr<API> sInstance = nullptr;



	void API::Initialise()
	{
		if (!sInstance)
		{
			sInstance = std::unique_ptr<API>(new API());
		}
	}



	API::~API()
	{
#if defined(_WIN32)
		WSACleanup();
#endif
	}



	API::API()
	{
#if defined(_WIN32)
		WSAData wsaData;
		auto err = WSAStartup(MAKEWORD(2, 2), &wsaData);
		Assert(err == 0);
#endif
	}
}
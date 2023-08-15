#include "Strawberry/Core/Net/Socket/SocketAPIUser.hpp"


#include <iostream>


namespace Strawberry::Core::Net::Socket
{
	std::weak_ptr<SocketAPI> SocketAPIUser::sGlobalAPI;


	SocketAPIUser::SocketAPIUser()
	{
		if (sGlobalAPI.expired())
		{
			mSocketAPI = std::shared_ptr<SocketAPI>(new SocketAPI());
			sGlobalAPI = mSocketAPI;
		}
		else
		{
			mSocketAPI = sGlobalAPI.lock();
		}
	}


	SocketAPIUser::~SocketAPIUser() {}
}
#pragma once


#include <memory>

#include "SocketAPI.hpp"


namespace Strawberry::Core::Net::Socket
{
	class SocketAPIUser
	{
	public:
		SocketAPIUser();
		virtual ~SocketAPIUser();
	private:
		static std::weak_ptr<SocketAPI> sGlobalAPI;
	private:
		std::shared_ptr<SocketAPI> mSocketAPI;
	};
}
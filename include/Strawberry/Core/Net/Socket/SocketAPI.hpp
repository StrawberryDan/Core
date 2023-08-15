#pragma once


namespace Strawberry::Core::Net::Socket
{
	class SocketAPI
	{
		friend class SocketAPIUser;


	public:
		~SocketAPI();


	private:
		SocketAPI();
	};
} // namespace Strawberry::Core::Net::Socket
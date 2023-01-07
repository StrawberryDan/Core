#pragma once



#include <variant>
#include "Address.hpp"



#include "Standard/Result.hpp"
#include "Error.hpp"



namespace Strawberry::Standard::Net
{
	class Endpoint
	{
	public:
		static Result<Endpoint, Error> Resolve(const std::string& hostname, uint16_t port);



	public:
		Endpoint(IPAddress address, uint16_t port);



		inline Option<std::string> GetHostname() const { return mHostName; };
		inline const IPAddress&    GetAddress()  const { return mAddress;  }
		inline        uint16_t     GetPort()     const { return mPort;     }


	private:
		Option<std::string> mHostName;
		IPAddress           mAddress;
		uint16_t            mPort;
	};
}
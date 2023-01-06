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


	private:
		IPAddress mAddress;
		uint16_t  mPort;
	};
}
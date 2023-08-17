#pragma once


#include "Address.hpp"
#include <variant>


#include "Error.hpp"
#include "Strawberry/Core/Util/Result.hpp"


namespace Strawberry::Core::Net
{
	class Endpoint
	{
	public:
		/// Resolves endpoints from a hostname and a port seperatly.
		static Result<Endpoint, Error> Resolve(const std::string& hostname, uint16_t port);
		/// Parses strings of the form <hostname>:<port> and resolves IP
		static Result<Endpoint, Error> Resolve(const std::string& endpoint);
		/// Parses strings of the form <hostname>:<port> without resolving IP
		static Result<Endpoint, Error> Parse(const std::string& endpoint);


	public:
		Endpoint(const std::string& hostname, uint16_t port);
		Endpoint(IPAddress address, uint16_t port);


		[[nodiscard]] inline const Option<std::string>& GetHostname() const { return mHostName; }


		[[nodiscard]] inline Option<IPAddress> GetAddress() const { return mAddress; }


		[[nodiscard]] inline uint16_t GetPort() const { return mPort; }


	private:
		Option<std::string> mHostName;
		Option<IPAddress>   mAddress;
		uint16_t            mPort;
	};
} // namespace Strawberry::Core::Net
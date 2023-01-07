#pragma once



#include "Standard/Net/Error.hpp"
#include "Standard/Option.hpp"
#include "TCPClient.hpp"
#include <memory>
#include <string>
#include <openssl/ssl.h>


namespace Strawberry::Standard::Net::Socket
{
	class TLSClient
	{
	public:
		static Result<TLSClient, Error> Connect(const Endpoint& endpoint);



	public:
		TLSClient();
		TLSClient(const TLSClient& other) = delete;
		TLSClient(TLSClient&& other);
		TLSClient& operator=(const TLSClient& other) = delete;
		TLSClient& operator=(TLSClient&& other);




	public:
		Result<IO::DynamicByteBuffer, IO::Error> Read(size_t length);
		Result<size_t, IO::Error>                Write(const IO::DynamicByteBuffer& bytes);



	private:
		TCPClient mTCP;
		SSL*      mSSL;
	};
}
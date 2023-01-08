#pragma once



#include <string>



#include "Standard/Net/Error.hpp"
#include "Standard/Result.hpp"
#include "Standard/Net/Endpoint.hpp"
#include "Standard/IO/DynamicByteBuffer.hpp"
#include "Standard/IO/Error.hpp"



#if defined(_WIN32)
#include <winsock2.h>
#endif



namespace Strawberry::Standard::Net::Socket
{
	class TCPClient
	{
		friend class TLSClient;
	public:
		static Result<TCPClient, Error> Connect(const Endpoint& endpoint);



	public:
		TCPClient();
		TCPClient(const TCPClient& other) = delete;
		TCPClient(TCPClient&& other);
		TCPClient& operator=(const TCPClient& other) = delete;
		TCPClient& operator=(TCPClient&& other);
		~TCPClient();



	public:
		Result<IO::DynamicByteBuffer, IO::Error> Read(size_t length);
		Result<size_t, IO::Error>                Write(const IO::DynamicByteBuffer& bytes);



	private:
#if defined(__APPLE__) || defined(__linux__)
		int    mSocket;
#elif defined(_WIN32)
		SOCKET mSocket;
#endif
	};
}
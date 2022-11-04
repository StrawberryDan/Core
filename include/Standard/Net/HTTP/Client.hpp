#pragma once



#include "Standard/Utilities.hpp"
#include "Standard/Net/Socket/SocketBase.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Standard/Net/Socket/TCPClient.hpp"
#include "Standard/Net/Socket/TLSClient.hpp"



namespace Strawberry::Standard::Net::HTTP
{
	template<Socket::SocketImpl S>
	class ClientImpl
	{
	public:
		explicit ClientImpl(const std::string& hostname, uint16_t port);



		inline S TakeSocket()
		{ return Take(mSocket); }



	public:
		void SendRequest(const Request& request);
		Response Receive();



	private:
		std::string ReadLine();
		ChunkedPayload ReadChunkedPayload();



	private:
		S mSocket;



	};



	class HTTPClient
		: public ClientImpl<Socket::TCPClient>
	{
	public:
		HTTPClient(const std::string& hostname, uint16_t port = 80);
	};



	class HTTPSClient
		: public ClientImpl<Socket::TLSClient>
	{
	public:
		HTTPSClient(const std::string& hostname, uint16_t port = 443);
	};
}



#include "Client.inl"
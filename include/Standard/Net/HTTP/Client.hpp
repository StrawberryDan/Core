#pragma once



#include "Standard/Utilities.hpp"
#include "Standard/Net/Socket/Socket.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Standard/Net/Socket/TCPClient.hpp"
#include "Standard/Net/Socket/TLSClient.hpp"



namespace Strawberry::Standard::Net::HTTP
{
	template<typename S> requires std::derived_from<S, Sockets::Socket>
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
		: public ClientImpl<Sockets::TCPClient>
	{
	public:
		HTTPClient(const std::string& hostname, uint16_t port = 80);
	};



	class HTTPSClient
		: public ClientImpl<Sockets::TLSClient>
	{
	public:
		HTTPSClient(const std::string& hostname, uint16_t port = 443);
	};
}



#include "Standard/Net/HTTP/Client.inl"
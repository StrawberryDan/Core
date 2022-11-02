#pragma once



#include "Standard/Utilities.hpp"
#include "Standard/Net/Socket/SocketBase.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Standard/Net/Socket/TCPClient.hpp"
#include "Standard/Net/Socket/TLSClient.hpp"



namespace Strawberry::Standard::Net::HTTP
{
	using namespace Socket;



	template<SocketImpl S, uint16_t PORT>
	class ClientImpl
	{
	public:
		explicit ClientImpl(const std::string& hostname);



		inline S TakeSocket()
		{ return Take(mSocket); }



		void SendRequest(const Request& request);
		Response Receive();

	private:
		std::string ReadLine();
		ChunkedPayload ReadChunkedPayload();

	private:
		S mSocket;
	};



	using HTTPClient  = ClientImpl<TCPClient, 80>;
	using HTTPSClient = ClientImpl<TLSClient, 443>;
}



#include "Client.inl"
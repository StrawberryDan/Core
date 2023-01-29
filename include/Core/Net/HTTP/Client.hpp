#pragma once



#include "Core/Utilities.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Core/Net/Socket/TCPClient.hpp"
#include "Core/Net/Socket/TLSClient.hpp"
#include "Core/IO/BufferedIO.hpp"



namespace Strawberry::Core::Net::HTTP
{
	template<typename S>
	class ClientBase
	{
	public:
		explicit ClientBase(const std::string& hostname, uint16_t port);



		inline IO::BufferedIO<S> TakeSocket()
		{ return Take(mSocket); }



	public:
		void SendRequest(const Request& request);
		Response Receive();



	private:
		std::string ReadLine();
		IO::DynamicByteBuffer ReadChunkedPayload();



	private:
		IO::BufferedIO<S> mSocket;



	};



	class HTTPClient
		: public ClientBase<Socket::TCPClient>
	{
	public:
		HTTPClient(const std::string& hostname, uint16_t port = 80);
	};



	class HTTPSClient
		: public ClientBase<Socket::TLSClient>
	{
	public:
		HTTPSClient(const std::string& hostname, uint16_t port = 443);
	};
}



#include "Core/Net/HTTP/Client.inl"
#pragma once


#include "Request.hpp"
#include "Response.hpp"
#include "Strawberry/Core/Net/Socket/TCPClient.hpp"
#include "Strawberry/Core/Net/Socket/TLSClient.hpp"
#include "Strawberry/Core/Util/Utilities.hpp"


namespace Strawberry::Core::Net::HTTP
{
	template <typename S>
	class ClientBase
	{
	public:
		/// Connects to the given endpoint over HTTP
		ClientBase(const std::string& hostname, uint16_t port);
		/// Sends an HTTP Request
		void     SendRequest(const Request& request);
		/// Waits for an HTTP Response
		Response Receive();


		/// Removes and returns the socket of an rvalue HTTP client.
		inline S TakeSocket() && { return std::move(mSocket); }


	private:
		/// Reads a line of input until a newline character from the socket.
		std::string           ReadLine();
		/// Reads a chunked HTTP payload from the socket.
		IO::DynamicByteBuffer ReadChunkedPayload();


	private:
		S mSocket;
	};


	class HTTPClient
		: public ClientBase<Socket::TCPClient>
	{
	public:
		explicit HTTPClient(const std::string& hostname, uint16_t port = 80);
	};


	class HTTPSClient
		: public ClientBase<Socket::TLSClient>
	{
	public:
		explicit HTTPSClient(const std::string& hostname, uint16_t port = 443);
	};
}// namespace Strawberry::Core::Net::HTTP


#include "Client.inl"

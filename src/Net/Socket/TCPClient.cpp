#include "Core/Net/Socket/TCPClient.hpp"



#include "Core/Assert.hpp"
#include "Core/Markers.hpp"
#include "Core/Net/Socket/API.hpp"
#include "Core/Utilities.hpp"
#include <iostream>



#if _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#elif __APPLE__ || __linux__
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#endif // _WIN32



namespace Strawberry::Core::Net::Socket
{
	Result<TCPClient, Error> TCPClient::Connect(const Endpoint& endpoint)
	{
		Socket::API::Initialise();

		TCPClient client;

		addrinfo hints {.ai_flags = AI_ADDRCONFIG, .ai_socktype = SOCK_STREAM,.ai_protocol = IPPROTO_TCP};
		if      (endpoint.GetAddress().IsIPv4()) hints.ai_family = AF_INET;
		else if (endpoint.GetAddress().IsIPv6()) hints.ai_family = AF_INET6;
		else                                     Unreachable();
		addrinfo* peerAddress = nullptr;
		auto addrResult = getaddrinfo(endpoint.GetAddress().AsString().c_str(),
									  std::to_string(endpoint.GetPort()).c_str(),
									  &hints, &peerAddress);
		if (addrResult != 0)
		{
			freeaddrinfo(peerAddress);
			return Error::AddressResolution;
		}

		auto handle = socket(peerAddress->ai_family, peerAddress->ai_socktype, peerAddress->ai_protocol);
		if (handle == -1)
		{
			freeaddrinfo(peerAddress);
			return Error::SocketCreation;
		}

		auto connection = connect(handle, peerAddress->ai_addr, peerAddress->ai_addrlen);
		if (connection == -1)
		{
			freeaddrinfo(peerAddress);
			return Error::EstablishConnection;
		}

		freeaddrinfo(peerAddress);
		client.mSocket = handle;
		return client;
	}



	TCPClient::TCPClient()
			: mSocket(-1)
	{}



	TCPClient::TCPClient(TCPClient&& other)
	{
		mSocket = Replace(other.mSocket, -1);
	}



	TCPClient& TCPClient::operator=(TCPClient&& other)
	{
		if (this != &other)
		{
			this->~TCPClient();
			mSocket = Replace(other.mSocket, -1);
		}

		return *this;
	}



	TCPClient::~TCPClient()
	{
		if (mSocket != -1)
		{
#if defined(__APPLE__) || defined(__linux__)
			close(mSocket);
#elif defined(__WIN32)
			closesocket(mSocket);
#endif
		}
	}



	Result<IO::DynamicByteBuffer, IO::Error> TCPClient::Read(size_t length)
	{
		auto buffer = IO::DynamicByteBuffer::Zeroes(length);
		auto bytesRead = recv(mSocket, reinterpret_cast<char*>(buffer.Data()), length, 0);

		if (bytesRead >= 0)
		{
			buffer.Resize(bytesRead);
			return buffer;
		}
		else
		{
			Unreachable();
		}
	}



	Result<size_t, IO::Error> TCPClient::Write(const IO::DynamicByteBuffer& bytes)
	{
		auto bytesSent = send(mSocket, reinterpret_cast<const char*>(bytes.Data()), bytes.Size(), 0);

		if (bytesSent >= 0)
		{
			return bytesSent;
		}
		else
		{
			Unreachable();
		}
	}
}
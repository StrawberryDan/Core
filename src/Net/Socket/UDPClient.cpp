#include "Standard/Net/Socket/UDPClient.hpp"



#include "Standard/Utilities.hpp"
#include "Standard/Assert.hpp"
#include "Standard/Markers.hpp"


#if _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#elif __APPLE__ || __linux__
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#endif // _WIN32



namespace Strawberry::Standard::Net::Socket
{
	Result<UDPClient, Error> UDPClient::Connect(const Endpoint& endpoint)
	{
		UDPClient client;

		addrinfo hints {.ai_flags = AI_ADDRCONFIG, .ai_socktype = SOCK_DGRAM,.ai_protocol = IPPROTO_UDP};
		if      (endpoint.GetAddress().IsIPv4()) hints.ai_family = AF_INET;
		else if (endpoint.GetAddress().IsIPv6()) hints.ai_family = AF_INET6;
		else                                     Unreachable();
		addrinfo* peerAddress = nullptr;
		auto addrResult = getaddrinfo(endpoint.GetAddress().AsString().c_str(),
		                              std::to_string(endpoint.GetPort()).c_str(),
		                              &hints, &peerAddress);
		if (addrResult != 0)
		{
			return Error::AddressResolution;
		}

		auto handle = socket(peerAddress->ai_family, peerAddress->ai_socktype, peerAddress->ai_protocol);
		if (handle == -1)
		{
			return Error::SocketCreation;
		}

		auto connection = connect(handle, peerAddress->ai_addr, peerAddress->ai_addrlen);
		if (connection == -1)
		{
			return Error::EstablishConnection;
		}

		freeaddrinfo(peerAddress);
		client.mSocket = handle;
		return client;
	}



	UDPClient::UDPClient()
			: mSocket(-1)
	{}



	UDPClient::UDPClient(UDPClient&& other)
		: mSocket(Replace(other.mSocket, -1))
	{

	}



	UDPClient& UDPClient::operator=(UDPClient&& other)
	{
		if (this != &other)
		{
			this->~UDPClient();
			mSocket = Replace(other.mSocket, -1);
		}

		return *this;
	}



	UDPClient::~UDPClient()
	{
		if (mSocket != -1)
		{
			close(mSocket);
		}
	}



	Result<IO::DynamicByteBuffer, IO::Error> UDPClient::Read()
	{
		auto bytesRead = recv(mSocket, mBuffer.Data(), BUFFER_SIZE, 0);

		if (bytesRead >= 0)
		{
			return IO::DynamicByteBuffer(mBuffer.Data(), bytesRead);
		}
		else
		{
			Unreachable();
		}
	}



	Result<size_t, IO::Error> UDPClient::Write(const IO::DynamicByteBuffer& bytes)
	{
		auto bytesWritten = send(mSocket, bytes.Data(), bytes.Size(), 0);

		if (bytesWritten >= 0)
		{
			return bytesWritten;
		}
		else
		{
			Unreachable();
		}
	}
}
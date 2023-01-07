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
	Result<UDPClient, Error> UDPClient::Create(uint16_t port)
	{
		addrinfo hints{.ai_flags = AI_ADDRCONFIG,.ai_socktype = SOCK_DGRAM, .ai_protocol = IPPROTO_UDP};
		addrinfo* peer = nullptr;
		auto getAddrResult = getaddrinfo("127.0.0.1", std::to_string(port).c_str(), &hints, &peer);
		if (getAddrResult != 0)
		{
			freeaddrinfo(peer);
			return Error::AddressResolution;
		}

		auto handle = socket(peer->ai_family, peer->ai_socktype, peer->ai_protocol);
		if (handle == -1)
		{
			freeaddrinfo(peer);
			return Error::SocketCreation;
		}

		UDPClient client;
		client.mSocket = handle;

		freeaddrinfo(peer);
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



	Result<std::tuple<Endpoint, IO::DynamicByteBuffer>, IO::Error> UDPClient::Read()
	{
		sockaddr_storage peer{};
		socklen_t peerLen = 0;
		auto bytesRead = recvfrom(mSocket, mBuffer.Data(), mBuffer.Size(), 0, reinterpret_cast<sockaddr*>(&peer), &peerLen);

		if (bytesRead >= 0)
		{
			Option<Endpoint> endpoint;
			if (peer.ss_family == AF_INET)
			{
				auto* sockaddr = reinterpret_cast<sockaddr_in*>(&peer);
				endpoint.Emplace(
						IPv4Address(IO::ByteBuffer<4>(sockaddr->sin_addr)),
						sockaddr->sin_port);
			}
			else if (peer.ss_family == AF_INET6)
			{
				auto* sockaddr = reinterpret_cast<sockaddr_in6*>(&peer);
				endpoint.Emplace(
						IPv6Address(IO::ByteBuffer<16>(sockaddr->sin6_addr)),
						sockaddr->sin6_port);
			}

			return std::make_tuple(endpoint.Unwrap(), IO::DynamicByteBuffer(mBuffer.Data(), bytesRead));
		}
		else
		{
			Unreachable();
		}
	}



	Result<size_t, IO::Error> UDPClient::Write(const Endpoint& endpoint, const IO::DynamicByteBuffer& bytes)
	{
		addrinfo hints{.ai_flags = AI_ADDRCONFIG, .ai_socktype = SOCK_DGRAM, .ai_protocol = IPPROTO_UDP};
		addrinfo* peer = nullptr;
		getaddrinfo(endpoint.GetAddress().AsString().c_str(),
					std::to_string(endpoint.GetPort()).c_str(),
					&hints,
					&peer);


		auto bytesSent = sendto(mSocket, bytes.Data(), bytes.Size(), 0, peer->ai_addr, peer->ai_addrlen);
		freeaddrinfo(peer);
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
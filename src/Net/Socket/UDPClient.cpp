
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
#else
#error "NO SOCKET IMPLEMENTATION FOR PLATFORM"
#endif // _WIN32



namespace Strawberry::Standard::Net::Socket
{
	UDPClient::UDPClient(const std::string& hostname, uint16_t port)
	    : mSocket{}
	{
#if _WIN32
	    WSAData wsaData;
	    WSAStartup(MAKEWORD(2,2), &wsaData);
	    mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	    Assert(mSocket != INVALID_SOCKET);

	    std::string portAsString = std::to_string(port);
	    addrinfo* addressInfo;
	    auto result = getaddrinfo(hostname.c_str(), portAsString.c_str(), nullptr, &addressInfo);
	    Assert(result == 0);

	    result = connect(mSocket, addressInfo->ai_addr, static_cast<int>(addressInfo->ai_addrlen));
	    Assert(result == 0);
#elif __APPLE__ || __linux__
		mSocket = socket(AF_INET, SOCK_DGRAM, 0);
		Assert(mSocket != -1);
		auto host = gethostbyname(hostname.c_str());
		Assert(host != nullptr);
		sockaddr_in address{};
		address.sin_family = AF_INET;
		address.sin_port = htons(port);
		memcpy(&address.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
		auto result = connect(mSocket, reinterpret_cast<sockaddr*>(&address), sizeof(address));
		Assert(result == 0);
#endif // _WIN32
	}



	UDPClient::UDPClient(UDPClient&& other) noexcept
	    : mSocket(Take(other.mSocket))
	{

	}



	UDPClient& UDPClient::operator=(UDPClient&& other) noexcept
	{
	    mSocket = Take(other.mSocket);
	    return (*this);
	}



	UDPClient::~UDPClient()
	{
#if _WIN32
	    closesocket(mSocket);
#elif __APPLE__ || __linux__
		if (mSocket)
		{
			auto err = shutdown(mSocket, SHUT_RDWR);
			Assert(err == 0);
		}
#endif // _WIN32
	}



	Result<size_t, SocketBase::Error> UDPClient::Read(uint8_t* data, size_t len) const
	{
#if _WIN32
	    auto bytesRead = recv(mSocket, reinterpret_cast<char*>(data), len, 0);
	    if (bytesRead == SOCKET_ERROR)
	    {
	        return Result<size_t, SocketBase::Error>::Err(SocketBase::ErrorFromWSACode(WSAGetLastError()));
	    }
	    else
	    {
	        return Result<size_t, SocketBase::Error>::Ok(static_cast<size_t>(bytesRead));
	    }
#elif __APPLE__ || __linux__
		auto bytesRead = recv(mSocket, data, len, IsBlocking() ? MSG_WAITALL : MSG_DONTWAIT);
		if (bytesRead >= 0)
		{
			return bytesRead;
		}
		else
		{
			switch (bytesRead)
			{
				default:
					Unreachable();
			}
		}
#endif // _WIN32
	}



	Result<size_t, SocketBase::Error> UDPClient::Write(const uint8_t* data, size_t len) const
	{
#if _WIN32
	    auto bytesSent = send(mSocket, reinterpret_cast<const char*>(data), len, 0);
	    if (bytesSent == SOCKET_ERROR)
	    {
	        return Result<size_t, SocketBase::Error>::Err(SocketBase::ErrorFromWSACode(WSAGetLastError()));
	    }
	    else
	    {
	        return Result<size_t, SocketBase::Error>::Ok(static_cast<size_t>(bytesSent));
	    }
#elif __APPLE__ || __linux__
		auto bytesSent = send(mSocket, data, len, 0);
		if (bytesSent >= 0)
		{
			return bytesSent;
		}
		else
		{
			switch (bytesSent)
			{
				default:
					Unreachable();
			}
		}
#endif // _WIN32
	}







	bool UDPClient::IsBlocking() const
	{
#if _WIN32

#elif __APPLE__ || __linux__
		auto flags = fcntl(mSocket, F_GETFL);
		Assert(flags >= 0);
		return !(flags & FNONBLOCK);
#else
	#warning "No Implementation for platform"
#endif
	}



	void UDPClient::SetBlocking(bool blocking)
	{
#if _WIN32

#elif __APPLE__ || __linux__
		auto flags = fcntl(mSocket, F_GETFL);
		Assert(flags >= 0);
		flags &= blocking ? ~FNONBLOCK : FNONBLOCK;
		auto result = fcntl(mSocket, F_SETFL, flags);
		Assert(result >= 0);
		Assert(IsBlocking() == blocking);
#else
	#warning "No Implementation for platform"
#endif
	}
}
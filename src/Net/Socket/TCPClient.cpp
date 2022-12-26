#include "Standard/Net/Socket/TCPClient.hpp"



#include "Standard/Utilities.hpp"
#include "Standard/Assert.hpp"
#include "Standard/Markers.hpp"
#include <iostream>



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



namespace Strawberry::Standard::Net::Sockets
{
	TCPClient::TCPClient(const std::string& hostname, uint16_t port)
	    : mSocket{}
	{
#if _WIN32
	    WSAData wsaData;
	    WSAStartup(MAKEWORD(2,2), &wsaData);
	    mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	    Assert(mSocket != INVALID_SOCKET);
	
	    std::string portAsString = std::to_string(port);
	    addrinfo* addressInfo;
	    auto result = getaddrinfo(hostname.c_str(), portAsString.c_str(), nullptr, &addressInfo);
	    Assert(result == 0);
	
	    result = connect(mSocket, addressInfo->ai_addr, static_cast<int>(addressInfo->ai_addrlen));
	    Assert(result == 0);
	
	    unsigned long iMode = 0;
	    result = ioctlsocket(mSocket, FIONBIO, &iMode);
	    Assert(result == 0);
	
	    int timeout = 1000;
	    result = setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));
	    Assert(result >= 0);
#elif __APPLE__ || __linux__
	    mSocket = socket(AF_INET, SOCK_STREAM, 0);
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
	
	
	
	TCPClient::TCPClient(TCPClient&& other) noexcept
	    : mSocket(Take(other.mSocket))
	{
	
	}
	
	
	
	TCPClient& TCPClient::operator=(TCPClient&& other) noexcept
	{
	    mSocket = Take(other.mSocket);
	    return (*this);
	}
	
	
	
	TCPClient::~TCPClient()
	{
#if _WIN32
	    if (mSocket)
	    {
	        shutdown(mSocket, SD_BOTH);
	        closesocket(mSocket);
	    }
#elif __APPLE__ || __linux__
	    if (mSocket)
	    {
	        auto err = shutdown(mSocket, SHUT_RDWR);
	        Assert(err == 0 || errno == ENOTCONN);
	    }
#endif // _WIN32
	}
	
	
	
	Result<size_t, Socket::Error> TCPClient::Read(uint8_t* data, size_t len) const
	{
#if _WIN32
	    auto bytesRead = recv(mSocket, reinterpret_cast<char*>(data), static_cast<int>(len), 0);
	    if (bytesRead == SOCKET_ERROR)
	    {
	
	        int wsaError = WSAGetLastError();
	        return Result<size_t, SocketBase::Error>::Err(SocketBase::ErrorFromWSACode(wsaError));
	    }
	    else
	    {
	        return Result<size_t, SocketBase::Error>::Ok(static_cast<size_t>(bytesRead));
	    }
#elif __APPLE__ || __linux__
		auto blocking = IsBlocking();
		auto flags = blocking ? MSG_WAITALL : MSG_DONTWAIT;
	    auto bytesRead = recv(mSocket, data, len, flags);
	    if (bytesRead >= 0)
	    {
	        return bytesRead;
	    }
		else
	    {
			switch (errno)
			{
				case EWOULDBLOCK:
					return Error::WouldBlock;
				default:
					Unreachable();
			}
	    }
#endif // _WIN32
	}
	
	
	
	Result<size_t, Socket::Error> TCPClient::Write(const uint8_t* data, size_t len) const
	{
#if _WIN32
	    auto bytesSent = send(mSocket, reinterpret_cast<const char*>(data), static_cast<int>(len), 0);
	    if (bytesSent == SOCKET_ERROR)
	    {
	        int wsaError = WSAGetLastError();
	        return Result<size_t, SocketBase::Error>::Err(SocketBase::ErrorFromWSACode(wsaError));
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
			switch (errno)
			{
				case EAGAIN:
					return Error::WouldBlock;
				default:
					Unreachable();
			}
		}
#endif // _WIN32
	}
	
	
	
	bool TCPClient::IsBlocking() const
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
	
	
	
	void TCPClient::SetBlocking(bool blocking)
	{
#if _WIN32
	
#elif __APPLE__ || __linux__
		auto flags = fcntl(mSocket, F_GETFL);
		Assert(flags >= 0);
		flags = blocking ? ~FNONBLOCK : FNONBLOCK;
		auto result = fcntl(mSocket, F_SETFL, flags);
		Assert(result >= 0);
		Assert(IsBlocking() == blocking);
#else
#warning "No Implementation for platform"
#endif
	}
}
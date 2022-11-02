#pragma once



#include "SocketBase.hpp"
#include <string>

#if _WIN32
using SOCKET_HANDLE = uintptr_t;
#elif __APPLE__ || __linux__
using SOCKET_HANDLE = int;
#else
#error "NO SOCKET IMPLEMENTATION FOR PLATFORM"
#endif // _WIN32



namespace Strawberry::Standard::Net::Socket
{
	class UDPClient : public SocketBase
	{
	public:
	    UDPClient(const std::string& hostname, uint16_t port);
	    UDPClient(const UDPClient&) = delete;
	    UDPClient& operator=(const UDPClient&) = delete;
	    UDPClient(UDPClient&& other) noexcept;
	    UDPClient& operator=(UDPClient&& other) noexcept;
	    ~UDPClient();

	    Result<size_t, SocketBase::Error> Read(uint8_t* data, size_t len) const override;
	    Result<size_t, SocketBase::Error> Write(const uint8_t* data, size_t len) const override;



		bool IsBlocking() const override;
		void SetBlocking(bool blocking) override;


	private:
	    SOCKET_HANDLE mSocket;
	};
}
#pragma once



#include "Socket.hpp"
#include <string>



namespace Strawberry::Standard::Net::Sockets
{
#if _WIN32
	using SOCKET_HANDLE = uintptr_t;
#elif __APPLE__ || __linux__
	using SOCKET_HANDLE = int;
#else
#error "NO SOCKET IMPLEMENTATION FOR PLATFORM"
#endif // _WIN32



	class UDPClient : public Socket
	{
	public:
	    UDPClient(const std::string& hostname, uint16_t port);
	    UDPClient(const UDPClient&) = delete;
	    UDPClient& operator=(const UDPClient&) = delete;
	    UDPClient(UDPClient&& other) noexcept;
	    UDPClient& operator=(UDPClient&& other) noexcept;
	    ~UDPClient();

	    Result<size_t, Socket::Error> Read(uint8_t* data, size_t len) const override;
	    Result<size_t, Socket::Error> Write(const uint8_t* data, size_t len) const override;



		bool IsBlocking() const override;
		void SetBlocking(bool blocking) override;


	private:
	    SOCKET_HANDLE mSocket;
	};
}
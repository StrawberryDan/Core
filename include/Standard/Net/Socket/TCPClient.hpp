#pragma once



#include <string>
#include "Socket.hpp"



#if _WIN32
using SOCKET_HANDLE = uintptr_t;
#elif __APPLE__ || __linux__
using SOCKET_HANDLE = int;
#else
#error "NO SOCKET IMPLEMENTATION FOR PLATFORM"
#endif // _WIN32



namespace Strawberry::Standard::Net::Sockets
{
	class TCPClient : public Socket
	{
	public:
		TCPClient(const std::string& hostname, uint16_t port);
		TCPClient(const TCPClient&) = delete;
		TCPClient& operator=(const TCPClient&) = delete;
		TCPClient(TCPClient&& other) noexcept;
		TCPClient& operator=(TCPClient&& other) noexcept;
		~TCPClient();

		Result<size_t, Error> Read(uint8_t* data, size_t len) const override;

		Result<size_t, Error> Write(const uint8_t* data, size_t len) const override;



		bool IsBlocking() const override;
		void SetBlocking(bool blocking) override;



	private:
		SOCKET_HANDLE mSocket;
	};
}
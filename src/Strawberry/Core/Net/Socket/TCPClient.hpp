#pragma once


#include <string>


#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
#include "Strawberry/Core/IO/Error.hpp"
#include "Strawberry/Core/Net/Endpoint.hpp"
#include "Strawberry/Core/Net/Error.hpp"
#include "Strawberry/Core/Types/Result.hpp"


#if defined(_WIN32)
#include <winsock2.h>
#endif


namespace Strawberry::Core::Net::Socket
{
	class TCPClient
	{
		friend class TLSClient;


	public:
		static Result<TCPClient, Error> Connect(const Endpoint& endpoint);


	public:
		TCPClient();
		TCPClient(const TCPClient& other) = delete;
		TCPClient(TCPClient&& other) noexcept;
		TCPClient& operator=(const TCPClient& other) = delete;
		TCPClient& operator=(TCPClient&& other) noexcept;
		~TCPClient();


		[[nodiscard]] bool                       Poll() const;
		Result<IO::DynamicByteBuffer, IO::Error> Read(size_t length);
		Result<size_t, IO::Error>                Write(const IO::DynamicByteBuffer& bytes);


	private:
#if defined(__APPLE__) || defined(__linux__)
		int mSocket;
#elif defined(_WIN32)
		SOCKET mSocket;
#endif
	};
} // namespace Strawberry::Core::Net::Socket
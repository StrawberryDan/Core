#pragma once



#include "Standard/IO/DynamicByteBuffer.hpp"
#include "Standard/IO/Error.hpp"
#include "Standard/Net/Endpoint.hpp"
#include "Standard/Result.hpp"
#include <tuple>



#if defined(_WIN32)
#include <winsock2.h>
#endif



namespace Strawberry::Standard::Net::Socket
{
	class UDPClient
	{
	public:
		static Result<UDPClient, Error> Create();
		static Result<UDPClient, Error> CreateIPv4();
		static Result<UDPClient, Error> CreateIPv6();



	public:
		UDPClient();
		UDPClient(const UDPClient& other) = delete;
		UDPClient(UDPClient&& other);
		UDPClient& operator=(const UDPClient& other) = delete;
		UDPClient& operator=(UDPClient&& other);
		~UDPClient();



	public:
		Result<std::tuple<Endpoint, IO::DynamicByteBuffer>, IO::Error> Read();
		Result<size_t, IO::Error>                Write(const Endpoint& endpoint, const IO::DynamicByteBuffer& bytes);



	private:
#if defined(__APPLE__) || defined(__linux__)
		int    mSocket;
#elif defined(_WIN32)
		SOCKET mSocket;
#endif

		static constexpr size_t BUFFER_SIZE = 25536;
		IO::ByteBuffer<BUFFER_SIZE> mBuffer;
	};
}
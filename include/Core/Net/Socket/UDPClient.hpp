#pragma once



#include "Core/IO/DynamicByteBuffer.hpp"
#include "Core/IO/Error.hpp"
#include "Core/Net/Endpoint.hpp"
#include "Core/Result.hpp"
#include <tuple>



#if defined(_WIN32)
#include <winsock2.h>
#endif



namespace Strawberry::Core::Net::Socket
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



		bool															Poll() const;
		Result<std::tuple<Endpoint, IO::DynamicByteBuffer>, IO::Error>	Read();
		Result<size_t, IO::Error>										Write(const Endpoint& endpoint, const IO::DynamicByteBuffer& bytes);



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
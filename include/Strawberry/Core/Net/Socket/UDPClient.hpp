#pragma once



#include "Strawberry/Core/IO/DynamicByteBuffer.hpp"
#include "Strawberry/Core/IO/Error.hpp"
#include "Strawberry/Core/Net/Endpoint.hpp"
#include "Strawberry/Core/Util/Result.hpp"
#include <tuple>
#include "SocketAPIUser.hpp"



#if defined(_WIN32)
#include <winsock2.h>
#endif



namespace Strawberry::Core::Net::Socket
{
	class UDPClient
		: private SocketAPIUser
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
		Result<std::tuple<Option<Endpoint>, IO::DynamicByteBuffer>, IO::Error>	Read();
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
#pragma once



#include <cstdint>
#include <string>
#include <optional>
#include <thread>
#include <future>



#include "Standard/Option.hpp"
#include "Standard/Result.hpp"
#include "Standard/Mutex.hpp"
#include "Standard/Net/Socket/SocketBase.hpp"
#include "Standard/Net/Socket/TCPClient.hpp"
#include "Standard/Net/Socket/TLSClient.hpp"
#include "Standard/Net/Websocket/Message.hpp"



namespace Strawberry::Standard::Net::Websocket
{
	enum class Error
	{
		Unknown,
		NoMessage,
		Closed,
		Refused,
	};



	template<Socket::SocketImpl S>
	class ClientImpl
	{
	public:
		ClientImpl(const ClientImpl&) = delete;
		ClientImpl& operator=(const ClientImpl&) = delete;
		ClientImpl(ClientImpl&& rhs) noexcept;
		ClientImpl& operator=(ClientImpl&& rhs) noexcept;

		~ClientImpl();



		void SendMessage(const Message& message);

		Result<Message, Error> ReadMessage();

		Result<Message, Error> WaitMessage();



		[[nodiscard]] inline bool IsValid() const
		{ return mSocket.HasValue(); }



		inline S TakeSocket()
		{ return Take(mSocket); }



	protected:
		using MessageBuffer = Mutex<std::vector<Message>>;
		using Fragment = std::pair<bool, Message>;



	protected:
		[[nodiscard]] Result<Message, Error> ReceiveFrame();
		[[nodiscard]] Result<Fragment, Error> ReceiveFragment();
		[[nodiscard]] Result<size_t, Error> TransmitFrame(const Message& frame);


		[[nodiscard]] static std::string GenerateNonce();
		[[nodiscard]] static uint8_t GetOpcodeMask(Message::Opcode opcode);
		[[nodiscard]] static Option<Message::Opcode> GetOpcodeFromByte(uint8_t byte);
		[[nodiscard]] static uint32_t GenerateMaskingKey();
		[[nodiscard]] static Error ErrorFromSocketError(typename S::Error err);



	protected:
		ClientImpl() = default;



	protected:
		Option<S> mSocket;
		Option<Error> mError;



	};



	class WSClient
		: public ClientImpl<Socket::TCPClient>
	{
	public:
		static Result<WSClient, Error> Connect(const std::string& host, const std::string& resource, uint16_t port = 80);
	};



	class WSSClient
		: public ClientImpl<Socket::TLSClient>
	{
	public:
		static Result<WSSClient, Error> Connect(const std::string& host, const std::string& resource, uint16_t port = 443);
	};
}



#include "Client.inl"
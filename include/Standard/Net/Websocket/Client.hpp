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
	template<Socket::SocketImpl S, uint16_t PORT>
	class ClientImpl
	{
	public:
		enum class Error;


	public:
		static Result<ClientImpl, Error> Connect(const std::string& host, const std::string& resource);

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



	private:
		using Fragment = std::pair<bool, Message>;


	private:
		ClientImpl() = default;



		[[nodiscard]] Result<Message, Error> ReceiveFrame();
		[[nodiscard]] Result<Fragment, Error> ReceiveFragment();
		[[nodiscard]] Result<size_t, Error> TransmitFrame(const Message& frame);


		[[nodiscard]] static std::string GenerateNonce();
		[[nodiscard]] static uint8_t GetOpcodeMask(Message::Opcode opcode);
		[[nodiscard]] static Option<Message::Opcode> GetOpcodeFromByte(uint8_t byte);
		[[nodiscard]] static uint32_t GenerateMaskingKey();
		[[nodiscard]] static Error ErrorFromSocketError(typename S::Error err);



	private:
		using MessageBuffer = Mutex<std::vector<Message>>;

		Option<S> mSocket;
		Option<Error> mError;
	};



	template<Socket::SocketImpl S, uint16_t PORT>
	enum class ClientImpl<S, PORT>::Error
	{
		Unknown,
		NoMessage,
		Closed,
		Refused,
	};



	using WSClient  = ClientImpl<Socket::TCPClient, 80>;
	using WSSClient = ClientImpl<Socket::TLSClient, 443>;
}



#include "Client.inl"
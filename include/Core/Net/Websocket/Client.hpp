#pragma once



#include <cstdint>
#include <string>
#include <optional>
#include <thread>
#include <future>



#include "Core/Option.hpp"
#include "Core/Result.hpp"
#include "Core/Mutex.hpp"
#include "Core/Net/Socket/TCPClient.hpp"
#include "Core/Net/Socket/TLSClient.hpp"
#include "Core/IO/BufferedIO.hpp"
#include "Core/Net/Websocket/Message.hpp"
#include "Core/IO/Concepts.hpp"



namespace Strawberry::Core::Net::Websocket
{
	enum class Error
	{
		Unknown,
		NoMessage,
		Closed,
		Refused,
		BadOp,
	};



	template<typename S> requires IO::Read<S> && IO::Write<S>
	class ClientBase
	{
	public:
		ClientBase(const ClientBase&)				= delete;
		ClientBase& operator=(const ClientBase&)	= delete;
		ClientBase(ClientBase&& rhs) 				= default;
		ClientBase& operator=(ClientBase&& rhs)		= default;
		~ClientBase();



		void Disconnect();



		void SendMessage(const Message& message);

		Result<Message, Error> ReadMessage();

		Result<Message, Error> WaitMessage();



		[[nodiscard]] inline bool IsValid() const
		{ return mSocket.HasValue(); }



		inline IO::BufferedReader<S> TakeSocket()
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



	protected:
		ClientBase() = default;



	protected:
		Option<IO::BufferedIO<S>>	mSocket;
		Option<Error>				mError;
	};



	class WSClient
		: public ClientBase<Socket::TCPClient>
	{
	public:
		static Result<WSClient, Error> Connect(const std::string& host, const std::string& resource, uint16_t port = 80);
	};



	class WSSClient
		: public ClientBase<Socket::TLSClient>
	{
	public:
		static Result<WSSClient, Error> Connect(const std::string& host, const std::string& resource, uint16_t port = 443);
	};
}



#include "Core/Net/Websocket/Client.inl"
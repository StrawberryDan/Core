#pragma once


#include <cstdint>
#include <future>
#include <optional>
#include <string>
#include <thread>


#include "Message.hpp"
#include "Strawberry/Core/IO/Concepts.hpp"
#include "Strawberry/Core/Net/Socket/TCPClient.hpp"
#include "Strawberry/Core/Net/Socket/TLSClient.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Util/Optional.hpp"
#include "Strawberry/Core/Util/Result.hpp"

namespace Strawberry::Core::Net::Websocket
{
	enum class Error
	{
		NoMessage,
		Closed,
		Refused,
		ProtocolError,
	};

	template <IO::ReadWrite S>
	class ClientBase
	{
	public:
		ClientBase(const ClientBase&)                    = delete;
		ClientBase& operator=(const ClientBase&)         = delete;
		ClientBase(ClientBase&& rhs) noexcept            = default;
		ClientBase& operator=(ClientBase&& rhs) noexcept = default;
		~ClientBase();


		Result<int, Error> SendMessage(const Message& message);

		Result<Message, Error> ReadMessage();

		Result<Message, Error> WaitMessage();

		[[nodiscard]] inline bool IsValid() const { return mSocket.HasValue(); }


	protected:
		using Fragment = std::pair<bool, Message>;


	protected:
		[[nodiscard]] Result<Message, Error>  ReceiveFrame();
		[[nodiscard]] Result<Fragment, Error> ReceiveFragment();
		[[nodiscard]] Result<size_t, Error>   TransmitFrame(const Message& frame);


		[[nodiscard]] static std::string               GenerateNonce();
		[[nodiscard]] static uint8_t                   GetOpcodeMask(Message::Opcode opcode);
		[[nodiscard]] static Optional<Message::Opcode> GetOpcodeFromByte(uint8_t byte);
		[[nodiscard]] static uint32_t                  GenerateMaskingKey();

		void Disconnect(int code = 1000);


	protected:
		ClientBase() = default;


	protected:
		Optional<Mutex<S>> mSocket;
		Optional<Error>    mError;
	};

	class WSClient : public ClientBase<Socket::TCPClient>
	{
	public:
		static Result<WSClient, Error> Connect(const std::string& host, const std::string& resource, uint16_t port = 80);
	};

	class WSSClient : public ClientBase<Socket::TLSClient>
	{
	public:
		static Result<WSSClient, Error> Connect(const std::string& host, const std::string& resource, uint16_t port = 443);
	};
} // namespace Strawberry::Core::Net::Websocket

#include "Client.inl"
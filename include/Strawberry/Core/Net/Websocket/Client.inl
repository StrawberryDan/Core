#pragma once



#include <random>
#include <future>



#include "Strawberry/Core/Net/HTTP/Client.hpp"
#include "Strawberry/Core/IO/Base64.hpp"
#include "Strawberry/Core/Endian.hpp"
#include "Strawberry/Core/Markers.hpp"
#include "Strawberry/Core/IO/Concepts.hpp"



namespace Strawberry::Core::Net::Websocket
{
	template<typename S> requires IO::Read<S> && IO::Write<S>
	ClientBase<S>::~ClientBase()
	{
		Disconnect();
	}



	template<typename S> requires IO::Read<S> && IO::Write<S>
	void ClientBase<S>::Disconnect()
	{
		if (mSocket)
		{
			auto code = ToBigEndian<uint16_t>(1000);
			Websocket::Message::Payload payload;
			payload.push_back(reinterpret_cast<uint8_t*>(&code)[0]);
			payload.push_back(reinterpret_cast<uint8_t*>(&code)[1]);
			SendMessage(Message(Message::Opcode::Close, payload)).Unwrap();
			while (true)
			{
				auto msg = ReadMessage();
				if (!msg && msg.Err() == Websocket::Error::Closed
					|| msg && msg.Unwrap().GetOpcode() == Message::Opcode::Close)
				{
					break;
				}
			}

			mSocket.Reset();
		}
	}



	template<typename S> requires IO::Read<S> && IO::Write<S>
	Result<int, Error> ClientBase<S>::SendMessage(const Message& message)
	{
		auto result = TransmitFrame(message);
		if (!result)
		{
			return result.Err();
		}
		else
		{
			return 0;
		}
	}



	template<typename S> requires IO::Read<S> && IO::Write<S>
	Result<Message, Error> ClientBase<S>::ReadMessage()
	{
		return ReceiveFrame();
	}



	template<typename S> requires IO::Read<S> && IO::Write<S>
	Result<Message, Error> ClientBase<S>::WaitMessage()
	{
		while (true)
		{
			if (auto msg = ReadMessage())
			{
				return Result<Message, Error>::Ok(msg.Unwrap());
			}
			else if (msg.Err() == Error::NoMessage)
			{
				continue;
			}
			else
			{
				return Result<Message, Error>::Err(msg.Err());
			}

			std::this_thread::yield();
		}
	}



	template<typename S> requires IO::Read<S> && IO::Write<S>
	std::string ClientBase<S>::GenerateNonce()
	{
		std::random_device randomDevice;
		IO::DynamicByteBuffer nonce(16);
		while (nonce.Size() < 16)
		{
			auto val = randomDevice();
			for (int i = 0; i < sizeof(val) && nonce.Size() < 16; i++)
			{
				nonce.Push(reinterpret_cast<uint8_t*>(&val)[i]);
			}
		}
		Assert(nonce.Size() == 16);
		auto base64 = IO::Base64::Encode(nonce);
		Assert(base64.size() == 24);
		return base64;
	}



	template<typename S> requires IO::Read<S> && IO::Write<S>
	uint8_t ClientBase<S>::GetOpcodeMask(Message::Opcode opcode)
	{
		switch (opcode)
		{
			case Message::Opcode::Continuation:
				return 0x0;
			case Message::Opcode::Text:
				return 0x1;
			case Message::Opcode::Binary:
				return 0x2;
			case Message::Opcode::Close:
				return 0x8;
			case Message::Opcode::Ping:
				return 0x9;
			case Message::Opcode::Pong:
				return 0xA;
			default:
				std::abort();
		}
	}



	template<typename S> requires IO::Read<S> && IO::Write<S>
	Option<Message::Opcode> ClientBase<S>::GetOpcodeFromByte(uint8_t byte)
	{
		using Opcode = Message::Opcode;

		switch (byte)
		{
			case 0x0:
				return Opcode::Continuation;
			case 0x1:
				return Opcode::Text;
			case 0x2:
				return Opcode::Binary;
			case 0x8:
				return Opcode::Close;
			case 0x9:
				return Opcode::Ping;
			case 0xA:
				return Opcode::Pong;
			default:
				DebugBreak();
				return {};
		}
	}



	template<typename S> requires IO::Read<S> && IO::Write<S>
	uint32_t ClientBase<S>::GenerateMaskingKey()
	{
		std::random_device rd;
		uint32_t key;
		for (int i = 0; i < sizeof(key); i++)
		{
			reinterpret_cast<uint8_t*>(&key)[i] = static_cast<uint8_t>(rd());
		}
		return key;
	}



	template<typename S> requires IO::Read<S> && IO::Write<S>
	Result<size_t, Error>
	ClientBase<S>::TransmitFrame(const Message& message)
	{
		if (!mSocket)
		{
			return Error::Closed;
		}

		auto socket = mSocket->Lock();
		Core::IO::DynamicByteBuffer bytesToSend;

		uint8_t byte = 0b10000000 | GetOpcodeMask(message.GetOpcode());
		bytesToSend.Push(byte);

		auto bytes = message.AsBytes();
		if (bytes.size() <= 125)
		{
			byte = 0b10000000 | static_cast<uint8_t>(bytes.size());
			bytesToSend.Push(byte);
		}
		else if (bytes.size() <= std::numeric_limits<uint16_t>::max())
		{
			byte = 0b10000000 | 126;
			bytesToSend.Push(byte);
			bytesToSend.Push(ToBigEndian(static_cast<uint16_t>(bytes.size())));
		}
		else if (bytes.size() <= std::numeric_limits<uint64_t>::max())
		{
			byte = 0b10000000 | 127;
			bytesToSend.Push(byte);
			bytesToSend.Push(ToBigEndian(static_cast<uint64_t>(bytes.size())));
		}

		uint32_t maskingKey = ToBigEndian(GenerateMaskingKey());
		bytesToSend.Push(maskingKey);

		for (int i = 0; i < bytes.size(); i++)
		{
			uint8_t mask = reinterpret_cast<uint8_t*>(&maskingKey)[i % sizeof(maskingKey)];
			bytesToSend.Push<uint8_t>(bytes[i] ^ mask);
		}

		auto sendResult = socket->Write(bytesToSend);
		if (sendResult)
		{
			return sendResult.Unwrap();
		}
		else switch (sendResult.Err())
		{
			default:
				Unreachable();
		}
	}



	template<typename S> requires IO::Read<S> && IO::Write<S>
	Result<Message, Error> ClientBase<S>::ReceiveFrame()
	{
		if (mError == Error::Closed)
		{
			return Result<Message, Error>::Err(*mError);
		}

		auto fragResult = ReceiveFragment();

		if (fragResult)
		{
			auto [final, message] = fragResult.Unwrap();

			while (!final)
			{
				auto fragResultB = ReceiveFragment();
				if (fragResultB)
				{
					auto [finalB, messageB] = fragResultB.Unwrap();
					message.Append(messageB);
					final = finalB;
				}
				else
				{
					return Result<Message, Error>::Err(fragResultB.Err());
				}
			}

			if (message.GetOpcode() == Message::Opcode::Close)
			{
				mSocket.Reset();
				mError = Error::Closed;
			}

			return Result<Message, Error>::Ok(std::move(message));
		}
		else
		{
			return Result<Message, Error>::Err(fragResult.Err());
		}
	}



	template<typename S> requires IO::Read<S> && IO::Write<S>
	Result<typename ClientBase<S>::Fragment, Error>
	ClientBase<S>::ReceiveFragment()
	{
		using Opcode = Message::Opcode;

		auto socket = mSocket->Lock();
		if (!socket->Poll())
		{
			return Error::NoMessage;
		}

		bool final;
		Opcode opcode;
		if (auto byte = socket->Read(1).template Map<uint8_t>([](auto x) -> uint8_t { return x.template Into<uint8_t>(); }))
		{
			final = *byte & 0b10000000;
			auto opcodeIn = GetOpcodeFromByte(*byte & 0b00001111);
			if (opcodeIn)
			{
				opcode = opcodeIn.Unwrap();
			}
			else
			{
				DebugBreak();
				return Error::BadOp;
			}
		}
		else switch (byte.Err())
		{
			case IO::Error::Closed:
				return Error::Closed;
			default:
				Unreachable();
		}

		bool masked;
		size_t size;
		if (auto byte = socket->Read(1).template Map<uint8_t>([](auto x) { return x.template Into<uint8_t>(); }))
		{
			masked = *byte & 0b10000000;
			Assert(!masked);
			uint8_t sizeByte = *byte & 0b01111111;
			if (sizeByte == 126)
			{
				size = FromBigEndian(socket->Read(sizeof(uint16_t)).Unwrap().template Into<uint16_t>());
			}
			else if (sizeByte == 127)
			{
				size = FromBigEndian(socket->Read(sizeof(uint64_t)).Unwrap().template Into<uint64_t>());
			}
			else
			{
				size = sizeByte;
			}
		}
		else
		{
			Unreachable();
		}


		std::vector<uint8_t> payload;
		if (size > 0)
		{
			payload = socket->Read(size).Unwrap().AsVector();
		}

		return Result<Fragment, Error>::Ok(final, Message(opcode, payload));
	}
}
#pragma once



#include <random>
#include <future>



#include "Standard/Net/HTTP/Client.hpp"
#include "Standard/Base64.hpp"
#include "Standard/Endian.hpp"
#include "Standard/Markers.hpp"



namespace
{
	using Strawberry::Standard::Assert;
};



namespace Strawberry::Standard::Net::Websocket
{
	template<Socket::SocketImpl S>
	ClientImpl<S>::ClientImpl(ClientImpl<S>&& rhs) noexcept
	{
		if (this != &rhs)
		{
			mSocket = Take(rhs.mSocket);
			mError = Take(rhs.mError);
		}
	}



	template<Socket::SocketImpl S>
	ClientImpl<S>& ClientImpl<S>::operator=(ClientImpl<S>&& rhs) noexcept
	{
		if (this != &rhs)
		{
			mSocket = Take(rhs.mSocket);
			mError = Take(rhs.mError);
		}

		return (*this);
	}



	template<Socket::SocketImpl S>
	ClientImpl<S>::~ClientImpl()
	{
		Disconnect();
	}



	template<Socket::SocketImpl S>
	void ClientImpl<S>::Disconnect()
	{
		if (mSocket)
		{
			auto code = ToBigEndian<uint16_t>(1000);
			Websocket::Message::Payload payload;
			payload.push_back(reinterpret_cast<uint8_t*>(&code)[0]);
			payload.push_back(reinterpret_cast<uint8_t*>(&code)[1]);
			SendMessage(Message(Message::Opcode::Close, payload));
			while (true)
			{
				auto msg = ReadMessage();
				if (msg && msg.Unwrap().GetOpcode() == Message::Opcode::Close)
				{
					break;
				}
			}

			mSocket.Reset();
		}
	}



	template<Socket::SocketImpl S>
	void ClientImpl<S>::SendMessage(const Message& message)
	{
		TransmitFrame(message).Unwrap();
	}



	template<Socket::SocketImpl S>
	Result<Message, Error> ClientImpl<S>::ReadMessage()
	{
		return ReceiveFrame();
	}



	template<Socket::SocketImpl S>
	Result<Message, Error> ClientImpl<S>::WaitMessage()
	{
		while (true)
		{
			auto msg = ReadMessage();
			if (msg)
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
		}
	}



	template<Socket::SocketImpl S>
	std::string ClientImpl<S>::GenerateNonce()
	{
		std::random_device randomDevice;
		std::vector<uint8_t> nonce;
		nonce.reserve(16);
		while (nonce.size() < 16)
		{
			auto val = randomDevice();
			for (int i = 0; i < sizeof(val) && nonce.size() < 16; i++)
			{
				nonce.push_back(reinterpret_cast<uint8_t*>(&val)[i]);
			}
		}
		Assert(nonce.size() == 16);
		auto base64 = Base64::Encode(nonce);
		Assert(base64.size() == 24);
		return base64;
	}



	template<Socket::SocketImpl S>
	uint8_t ClientImpl<S>::GetOpcodeMask(Message::Opcode opcode)
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



	template<Socket::SocketImpl S>
	Option<Message::Opcode> ClientImpl<S>::GetOpcodeFromByte(uint8_t byte)
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
				return {};
		}
	}



	template<Socket::SocketImpl S>
	uint32_t ClientImpl<S>::GenerateMaskingKey()
	{
		std::random_device rd;
		uint32_t key;
		for (int i = 0; i < sizeof(key); i++)
		{
			reinterpret_cast<uint8_t*>(&key)[i] = static_cast<uint8_t>(rd());
		}
		return key;
	}



	template<Socket::SocketImpl S>
	Error ClientImpl<S>::ErrorFromSocketError(typename S::Error err)
	{
		switch (err)
		{
			case S::Error::Closed:
				return Error::Closed;
			case S::Error::WouldBlock:
				return Error::NoMessage;
			default:
				Unreachable();
		}
	}



	template<Socket::SocketImpl S>
	Result<size_t, Error>
	ClientImpl<S>::TransmitFrame(const Message& message)
	{
		size_t bytesTransmitted = 0;

		uint8_t byte = 0b10000000;
		byte |= GetOpcodeMask(message.GetOpcode());
		bytesTransmitted += mSocket->WriteType(byte).Unwrap();

		byte = 0b10000000;
		auto bytes = message.AsBytes();
		if (bytes.size() <= 125)
		{
			byte |= static_cast<uint8_t>(bytes.size());
			bytesTransmitted += mSocket->WriteType(byte).Unwrap();
		}
		else if (bytes.size() <= std::numeric_limits<uint16_t>::max())
		{
			byte |= 126;
			bytesTransmitted += mSocket->WriteType(byte).Unwrap();
			bytesTransmitted += mSocket->WriteType(ToBigEndian(static_cast<uint16_t>(bytes.size()))).Unwrap();
		}
		else if (bytes.size() <= std::numeric_limits<uint64_t>::max())
		{
			byte |= 127;
			bytesTransmitted += mSocket->WriteType(byte).Unwrap();
			bytesTransmitted += mSocket->WriteType(ToBigEndian(static_cast<uint64_t>(bytes.size()))).Unwrap();
		}

		uint32_t maskingKey = ToBigEndian(GenerateMaskingKey());
		static_assert(sizeof(maskingKey) == 4);
		bytesTransmitted += mSocket->WriteType(maskingKey).Unwrap();

		for (int i = 0; i < bytes.size(); i++)
		{
			auto mask = reinterpret_cast<uint8_t*>(&maskingKey)[i % sizeof(maskingKey)];
			bytesTransmitted += mSocket->template WriteType<uint8_t>(bytes[i] ^ mask).Unwrap();
		}

		return Result<size_t, Error>::Ok(bytesTransmitted);
	}



	template<Socket::SocketImpl S>
	Result<Message, Error> ClientImpl<S>::ReceiveFrame()
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
				mSocket = {};
				mError = Error::Closed;
			}

			return Result<Message, Error>::Ok(std::move(message));
		}
		else
		{
			return Result<Message, Error>::Err(fragResult.Err());
		}
	}



	template<Socket::SocketImpl S>
	Result<typename ClientImpl<S>::Fragment, Error>
	ClientImpl<S>::ReceiveFragment()
	{
		using Opcode = Message::Opcode;

		mSocket->SetBlocking(false);

		bool final;
		Opcode opcode;
		if (auto byte = mSocket->template ReadType<uint8_t>())
		{
			final = *byte & 0b10000000;
			auto opcodeIn = GetOpcodeFromByte(*byte & 0b00001111);
			if (opcodeIn)
			{
				opcode = opcodeIn.Unwrap();
			}
			else
			{
				return Error::BadOp;
			}
		}
		else
		{
			return Result<Fragment, Error>::Err(ErrorFromSocketError(byte.Err()));
		}

		mSocket->SetBlocking(true);

		bool masked;
		size_t size;
		if (auto byte = mSocket->template ReadType<uint8_t>())
		{
			masked = *byte & 0b10000000;
			Assert(!masked);
			uint8_t sizeByte = *byte & 0b01111111;
			if (sizeByte == 126)
			{
				size = FromBigEndian(mSocket->template ReadType<uint16_t>().Unwrap());
			}
			else if (sizeByte == 127)
			{
				size = FromBigEndian(mSocket->template ReadType<uint64_t>().Unwrap());
			}
			else
			{
				size = sizeByte;
			}
		}
		else
		{
			return Result<Fragment, Error>::Err(ErrorFromSocketError(byte.Err()));
		}


		std::vector<uint8_t> payload;
		if (size > 0)
		{
			payload = mSocket->template ReadVector<uint8_t>(size).Unwrap();
		}

		return Result<Fragment, Error>::Ok(final, Message(opcode, payload));
	}
}
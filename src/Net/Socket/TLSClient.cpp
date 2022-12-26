#include "Standard/Net/Socket/TLSClient.hpp"



#include "Standard/Assert.hpp"
#include "Standard/Log.hpp"
#include "Standard/Utilities.hpp"



namespace Strawberry::Standard::Net::Socket
{
	TLSClient::TLSClient(const std::string& host, uint16_t port)
			: mTLS(nullptr), mConfig(nullptr), mCallbackArgs(std::make_unique<CallbackArg>(
			std::make_tuple<TCPClient, Option<Result<size_t, Socket::Error>>, Option<Result<size_t, Socket::Error>>>(
					{host, port}, {}, {})))
	{
		auto result = tls_init();
		Assert(result >= 0);
		mTLS = tls_client();
		Assert(mTLS != nullptr);
		mConfig = tls_config_new();
		Assert(mConfig != nullptr);
		result = tls_config_set_protocols(mConfig, TLS_PROTOCOL_TLSv1_2);
		Assert(result >= 0);
		result = tls_configure(mTLS, mConfig);
		Assert(result >= 0);
		auto portAsString = std::to_string(port);
		result = tls_connect_cbs(mTLS, RecvData, SendData, mCallbackArgs.get(), host.c_str());
		Assert(result >= 0);
		result = tls_handshake(mTLS);
		Assert(result >= 0);
	}



	TLSClient::TLSClient(TLSClient&& rhs) noexcept
			: mTLS(Take(rhs.mTLS)), mConfig(Take(rhs.mConfig)), mCallbackArgs(Take(rhs.mCallbackArgs))
	{
	}



	TLSClient& TLSClient::operator=(TLSClient&& rhs) noexcept
	{
		if (this != &rhs)
		{
			mTLS = Take(rhs.mTLS);
			mConfig = Take(rhs.mConfig);
			mCallbackArgs = Take(rhs.mCallbackArgs);
		}

		return (*this);
	}



	TLSClient::~TLSClient()
	{
		if (mConfig) tls_config_free(mConfig);
		if (mTLS)
		{
			tls_close(mTLS);
			tls_free(mTLS);
		}
	}



	Result<size_t, Socket::Error> TLSClient::Read(uint8_t* data, size_t len) const
	{
		tls_read(mTLS, data, len);
		Assert(std::get<1>(*mCallbackArgs).HasValue());
		auto ret = *std::get<1>(*mCallbackArgs);
		return ret;
	}



	Result<size_t, Socket::Error> TLSClient::Write(const uint8_t* data, size_t len) const
	{
		tls_write(mTLS, data, len);
		Assert(std::get<2>(*mCallbackArgs).HasValue());
		auto ret = *std::get<2>(*mCallbackArgs);
		return ret;
	}



	bool TLSClient::IsBlocking() const
	{
		return std::get<TCPClient>(*mCallbackArgs).IsBlocking();
	}



	void TLSClient::SetBlocking(bool blocking)
	{
		std::get<TCPClient>(*mCallbackArgs).SetBlocking(blocking);
	}



	ssize_t TLSClient::SendData(tls* tls, const void* data, size_t len, void* _args)
	{
		auto args = reinterpret_cast<CallbackArg*>(_args);
		auto& [socket, read, write] = *args;
		write = socket.Write(static_cast<const uint8_t*>(data), len);

		if (*write)
		{
			return static_cast<ssize_t>(**write);
		}
		else
		{
			return 0;
		}
	}



	ssize_t TLSClient::RecvData(tls* tls, void* data, size_t len, void* _args)
	{
		auto args = reinterpret_cast<CallbackArg*>(_args);
		auto& [socket, read, write] = *args;
		read = socket.Read(static_cast<uint8_t*>(data), len);

		if (*read)
		{
			return static_cast<ssize_t>(**read);
		}
		else
		{
			return 0;
		}
	}
}
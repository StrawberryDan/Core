#pragma once



#include "tls.h"
#include <string>
#include <memory>
#include "SocketBase.hpp"
#include "TCPClient.hpp"
#include "Standard/Option.hpp"



namespace Strawberry::Standard::Net::Socket
{
	class TLSClient : public SocketBase
	{
	public:
	    TLSClient(const std::string& host, uint16_t port);
	    TLSClient(const TLSClient&) = delete;
	    TLSClient& operator=(const TLSClient&) = delete;
	    TLSClient(TLSClient&& rhs) noexcept ;
	    TLSClient& operator=(TLSClient&& other) noexcept ;
	    ~TLSClient();


	    Result<size_t, Error> Read(uint8_t* data, size_t len) const override;
	    Result<size_t, Error> Write(const uint8_t* data, size_t len) const override;


		bool IsBlocking() const override;
		void SetBlocking(bool blocking) override;



	private:
		using CallbackArg = std::tuple<TCPClient, Option<Result<size_t, SocketBase::Error>>, Option<Result<size_t, SocketBase::Error>>>;



	private:
		static ssize_t SendData(tls* tls, const void* data, size_t len, void* args);
		static ssize_t RecvData(tls* tls,       void* data, size_t len, void* args);



	private:
	    tls*                         mTLS;
	    tls_config*                  mConfig;
		std::unique_ptr<CallbackArg> mCallbackArgs;
	};
}
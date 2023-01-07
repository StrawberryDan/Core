#include "Standard/Net/Socket/TLSClient.hpp"



#include "Standard/Assert.hpp"
#include "Standard/Log.hpp"
#include "Standard/Utilities.hpp"
#include "Standard/Markers.hpp"
#include <memory>
#include <openssl/tls1.h>




class TLSContext
{
public:
	static SSL_CTX* Get()
	{
		if (!mInstance)
		{
			mInstance = std::unique_ptr<TLSContext>(new TLSContext());
		}

		return mInstance.get()->mSSL_CONTEXT;
	}



	~TLSContext()
	{
		SSL_CTX_free(mSSL_CONTEXT);
	}



private:
	TLSContext()
		: mSSL_CONTEXT(nullptr)
	{
		SSL_library_init();
		OpenSSL_add_all_algorithms();
		SSL_load_error_strings();

		mSSL_CONTEXT = SSL_CTX_new(TLS_client_method());
		Strawberry::Standard::Assert(mSSL_CONTEXT != nullptr);
	}

	SSL_CTX* mSSL_CONTEXT;

	static std::unique_ptr<TLSContext> mInstance;
};



std::unique_ptr<TLSContext> TLSContext::mInstance = nullptr;



namespace Strawberry::Standard::Net::Socket
{
	Result<TLSClient, Error> TLSClient::Connect(const Endpoint& endpoint)
	{
		auto tcp = TCPClient::Connect(endpoint);
		if (!tcp)
		{
			return tcp.Err();
		}

		auto ssl = SSL_new(TLSContext::Get());
		if (ssl == nullptr)
		{
			return Error::SSLAllocation;
		}

		if (endpoint.GetHostname())
		{
			auto hostnameResult = SSL_set_tlsext_host_name(ssl, endpoint.GetHostname()->c_str());
			Assert(hostnameResult);
		}

		SSL_set_fd(ssl, tcp->mSocket);
		auto connectResult = SSL_connect(ssl);
		if (connectResult == -1)
		{
			return Error::SSLHandshake;
		}

		TLSClient tls;
		tls.mTCP = tcp.Unwrap();
		tls.mSSL = ssl;
		return tls;
	}



	TLSClient::TLSClient()
		: mSSL(nullptr)
	{
		if (mSSL)
		{
			SSL_shutdown(mSSL);
			SSL_free(mSSL);
		}
	}



	TLSClient::TLSClient(TLSClient&& other)
	{
		mSSL = Take(other.mSSL);
		mTCP = Take(other.mTCP);
	}



	TLSClient& TLSClient::operator=(TLSClient&& other)
	{
		if (this != &other)
		{
			this->~TLSClient();
			mSSL = Take(other.mSSL);
			mTCP = Take(other.mTCP);
		}

		return *this;
	}



	Result<IO::DynamicByteBuffer, IO::Error> TLSClient::Read(size_t length)
	{
		auto buffer = IO::DynamicByteBuffer::Zeroes(length);
		auto bytesRead = SSL_read(mSSL, reinterpret_cast<void*>(buffer.Data()), length);

		if (bytesRead >= 0)
		{
			buffer.Resize(bytesRead);
			return buffer;
		}
		else
		{
			Unreachable();
		}
	}



	Result<size_t, IO::Error> TLSClient::Write(const IO::DynamicByteBuffer& bytes)
	{
		auto bytesSent = SSL_write(mSSL, bytes.Data(), bytes.Size());

		if (bytesSent >= 0)
		{
			return bytesSent;
		}
		else
		{
			Unreachable();
		}
	}
}
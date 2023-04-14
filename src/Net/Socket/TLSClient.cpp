#include "Core/Net/Socket/TLSClient.hpp"



#include "Core/Assert.hpp"
#include "Core/Net/Socket/API.hpp"
#include "Core/Utilities.hpp"
#include <memory>
#include <openssl/tls1.h>



#if defined(__APPLE__) || defined(__linux__)
#include <sys/socket.h>
#include <unistd.h>
#endif



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
		Strawberry::Core::Assert(mSSL_CONTEXT != nullptr);
	}

	SSL_CTX* mSSL_CONTEXT;

	static std::unique_ptr<TLSContext> mInstance;
};



std::unique_ptr<TLSContext> TLSContext::mInstance = nullptr;



namespace Strawberry::Core::Net::Socket
{
	Result<TLSClient, Error> TLSClient::Connect(const Endpoint& endpoint)
	{
		API::Initialise();


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
	{}



	TLSClient::TLSClient(TLSClient&& other)
	{
		mTCP = std::move(other.mTCP);
		mSSL = std::exchange(other.mSSL, nullptr);
	}



	TLSClient& TLSClient::operator=(TLSClient&& other)
	{
		if (this != &other)
		{
			std::destroy_at(this);
			std::construct_at(this, std::move(other));
		}

		return *this;
	}



	TLSClient::~TLSClient()
	{
		if (mSSL)
		{
			SSL_shutdown(mSSL);

#if defined(__APPLE__) || defined(__linux__)
			close(mTCP.mSocket);
#elif defined(_WIN32)
			closesocket(mTCP.mSocket);
#endif

			SSL_free(mSSL);
		}
	}



	bool TLSClient::Poll() const
	{
		return mTCP.Poll();
	}



	Result<IO::DynamicByteBuffer, IO::Error> TLSClient::Read(size_t length)
	{
		auto buffer = IO::DynamicByteBuffer::Zeroes(length);
		auto bytesRead = SSL_read(mSSL, reinterpret_cast<void*>(buffer.Data()), static_cast<int>(length));

		if (bytesRead > 0)
		{
			buffer.Resize(bytesRead);
			return buffer;
		}
		else
		{
            auto error = SSL_get_error(mSSL, bytesRead);
			switch (error)
			{
                case SSL_ERROR_ZERO_RETURN:
                    return IO::Error::Closed;
                case SSL_ERROR_SYSCALL:
                    std::cerr << "SSL read error. Syscall: " << strerror(errno) << std::endl;
                    return IO::Error::Syscall;
				default:
                    std::cerr << "Unknown SSL_read error code: " << error << std::endl;
                    return IO::Error::Unknown;
			}
		}
	}



	Result<size_t, IO::Error> TLSClient::Write(const IO::DynamicByteBuffer& bytes)
	{
		auto bytesSent = SSL_write(mSSL, bytes.Data(), static_cast<int>(bytes.Size()));

		if (bytesSent >= 0)
		{
			return bytesSent;
		}
		else
		{
			switch (SSL_get_error(mSSL, bytesSent))
			{
				default:
					return IO::Error::Unknown;
			}
		}
	}
}

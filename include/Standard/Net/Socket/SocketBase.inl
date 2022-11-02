#include "Standard/Assert.hpp"



namespace Strawberry::Standard::Net::Socket
{
	template<typename T>
	Result<T, SocketBase::Error> SocketBase::ReadType() const requires ( std::is_fundamental_v<T> )
	{
	    T data;
	    auto readResult = Read(reinterpret_cast<uint8_t*>(&data), sizeof(T));
	    if (readResult)
	    {
	        return Result<T, Error>::Ok(data);
	    }
	    else
	    {
	        return Result<T, Error>::Err(readResult.Err());
	    }
	}



	template<typename T>
	Result<size_t, SocketBase::Error> SocketBase::ReadArray(T* data, size_t count) const requires ( std::is_fundamental_v<T> )
	{
	    auto readResult = Read(reinterpret_cast<uint8_t*>(data), count * sizeof(T));
	    if (readResult)
	    {
	        return Result<T, Error>::Ok(data);
	    }
	    else
	    {
	        return Result<T, Error>::Err(readResult);
	    }
	}



	template<typename T>
	Result<std::vector<T>, SocketBase::Error> SocketBase::ReadVector(size_t count) const requires ( std::is_fundamental_v<T> )
	{
	    std::vector<T> data;
	    data.resize(count);
	    auto readResult = Read(data.data(), count);
	    if (readResult)
	    {
	        return Result<std::vector<T>, SocketBase::Error>::Ok(data);
	    }
	    else
	    {
	        return Result<std::vector<T>, SocketBase::Error>::Err(readResult.Err());
	    }
	}



	template<typename T>
	Result<size_t, SocketBase::Error> SocketBase::WriteType(const T& data) const requires ( std::is_fundamental_v<T> )
	{
	    return Write(reinterpret_cast<const uint8_t*>(&data), sizeof(T));
	}



	template<typename T>
	Result<size_t, SocketBase::Error> SocketBase::WriteArray(const T* data, size_t count) const requires ( std::is_fundamental_v<T> )
	{
	    return Write(reinterpret_cast<const uint8_t*>(data), count * sizeof(T));
	}



	template<typename T>
	Result<size_t, SocketBase::Error> SocketBase::WriteVector(const std::vector<T>& data) const requires ( std::is_fundamental_v<T> )
	{
	    return Write(reinterpret_cast<const uint8_t*>(data.data()), data.size() * sizeof(T));
	}
}
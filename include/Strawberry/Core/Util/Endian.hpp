#pragma once



#include <bit>
#include <type_traits>



namespace Strawberry::Core
{
	template<typename T> requires ( std::is_fundamental_v<T> )
	T Byteswap(T in)
	{
	    T out = 0;
	    for (int i = 0; i < sizeof(T); i++)
	    {
	        reinterpret_cast<uint8_t*>(&out)[sizeof(T) - i - 1] = reinterpret_cast<uint8_t*>(&in)[i];
	    }
	    return out;
	}



	template<typename T>
	T ToBigEndian(T v) requires( std::is_integral_v<T> )
	{
	    if constexpr (std::endian::native == std::endian::big)
	    {
	        return v;
	    }
	    else if (std::endian::native == std::endian::little)
	    {
	        return Byteswap(v);
	    }
	}



	template<typename T>
	T ToLittleEndian(T v) requires( std::is_integral_v<T> )
	{
	    if constexpr (std::endian::native == std::endian::little)
	    {
	        return v;
	    }
	    else if (std::endian::native == std::endian::big)
	    {
	        return Byteswap(v);
	    }
	}



	template<typename T>
	T FromBigEndian(T v) requires( std::is_integral_v<T> )
	{
	    if constexpr (std::endian::native == std::endian::big)
	    {
	        return v;
	    }
	    else if (std::endian::native == std::endian::little)
	    {
	        return Byteswap(v);
	    }
	}



	template<typename T>
	T FromLittleEndian(T v) requires( std::is_integral_v<T> )
	{
	    if constexpr (std::endian::native == std::endian::little)
	    {
	        return v;
	    }
	    else if (std::endian::native == std::endian::big)
	    {
	        return Byteswap(v);
	    }
	}
}
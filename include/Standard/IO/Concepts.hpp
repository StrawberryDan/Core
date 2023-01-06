#pragma once



#include <cstdlib>



#include "DynamicByteBuffer.hpp"
#include "Error.hpp"
#include "Standard/Result.hpp"



namespace Strawberry::Standard::IO
{
	template <typename T>
	concept Read = requires(T t, size_t len)
	{
		{ t.Read(len) } -> std::same_as<Result<DynamicByteBuffer, Error>>;
	};



	template <typename T>
	concept Write = requires(T t, const DynamicByteBuffer& bytes)
	{
		{ t.Write(bytes) } -> std::same_as<Result<size_t, Error>>;
	};
}
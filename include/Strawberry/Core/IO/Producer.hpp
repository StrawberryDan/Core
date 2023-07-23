#pragma once



#include "Strawberry/Core/Option.hpp"



namespace Strawberry::Core::IO
{
	template <typename T>
	class Producer
	{
	public:
		Option<T> Receive() = 0;
	};
}

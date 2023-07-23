#pragma once


#include <cstddef>
#include "Consumer.hpp"
#include "Producer.hpp"


namespace Strawberry::Core::IO
{
	class Transformer
	{
	public:
		template<typename T>
		Consumer<T>* GetInputPad(size_t index)
		{ return nullptr; }


		template<typename T>
		Producer<T>* GetOutputPad(size_t index)
		{ return nullptr; }
	};
}
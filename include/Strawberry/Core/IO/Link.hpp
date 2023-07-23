#pragma once


#include "Producer.hpp"
#include "Consumer.hpp"


namespace Strawberry::Core::IO
{
	template<typename T>
	class Link
	{
	public:
		bool Tick()
		{
			auto input = mProducer->Receive();
			if (input)
			{
				return mConsumer->Feed(std::move(input.Unwrap()));
			}

			return false;
		}


	private:
		Producer<T>* mProducer;
		Consumer<T>* mConsumer;
	};
}
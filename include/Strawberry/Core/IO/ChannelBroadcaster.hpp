#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "ChannelReceiver.hpp"
// Standard Library
#include <vector>
#include <memory>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template<typename T>
	class ChannelBroadcaster
	{
	public:
		void Broadcast(T value)
		{
			std::erase_if(mReceivers, [](const std::weak_ptr<ChannelReceiver<T>>& x) { return x.expired(); });
			for (auto& receiver: mReceivers)
			{
				receiver.lock()->Receive(value);
			}
		}


		std::shared_ptr<ChannelReceiver<T>> CreateReceiver()
		{
			auto receiver = std::shared_ptr<ChannelReceiver<T>>(new ChannelReceiver<T>());
			mReceivers.emplace_back(receiver);
			return receiver;
		}


	private:
		std::vector<std::weak_ptr<ChannelReceiver<T>>> mReceivers;
	};
}
#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "ChannelReceiver.hpp"
// Standard Library
#include <map>
#include <memory>
#include <set>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template<typename T, typename... Ts>
	class ChannelBroadcaster
			: public ChannelBroadcaster<T>, public ChannelBroadcaster<Ts...>
	{
	public:
		using ChannelBroadcaster<T>::Broadcast;
		using ChannelBroadcaster<Ts>::Broadcast...;
		using ChannelBroadcaster<T>::Register;
		using ChannelBroadcaster<Ts>::Register...;
		using ChannelBroadcaster<T>::Unregister;
		using ChannelBroadcaster<Ts>::Unregister...;
	};


	template<typename T>
	class ChannelBroadcaster<T>
	{
		template<typename, typename...>
		friend
		class ChannelBroadcaster;

	public:
		ChannelBroadcaster() = default;

		ChannelBroadcaster(const ChannelBroadcaster& rhs)            = delete;
		ChannelBroadcaster& operator=(const ChannelBroadcaster& rhs) = delete;
		ChannelBroadcaster(ChannelBroadcaster&& rhs)                 = default;
		ChannelBroadcaster& operator=(ChannelBroadcaster&& rhs)      = delete;


		void Register(ChannelReceiver<T>& receiver)
		{
			mReceivers.emplace(receiver.GetReflexivePointer());
		}


		void Unregister(ChannelReceiver<T>& receiver)
		{
			mReceivers.erase(receiver.GetReflexivePointer());
		}


		void Broadcast(const T& value)
		{
			std::set<ReflexivePointer<ChannelReceiver<T>>> toRemove;

			for (auto& receiver: mReceivers)
			{
				if (receiver) receiver->Receive(value);
				else toRemove.emplace(receiver);
			}

			for (auto& receiver: toRemove)
			{
				mReceivers.erase(receiver);
			}
		}

	protected:
		std::set<ReflexivePointer<ChannelReceiver<T>>> mReceivers;
	};
} // namespace Strawberry::Core::IO

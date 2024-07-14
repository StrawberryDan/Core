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
	template<std::copyable T, std::copyable... Ts>
	class ChannelBroadcaster
			: protected ChannelBroadcaster<T>, protected ChannelBroadcaster<Ts...>
	{
	public:
		using ChannelBroadcaster<T>::Broadcast;
		using ChannelBroadcaster<Ts>::Broadcast...;


		template<typename R, typename... Rs>
		void Register(ChannelReceiver<R, Rs...>* receiver)
		{
			if constexpr (std::is_same_v<R, T> || (std::is_same_v<R, Ts> || ...))
			{
				ChannelBroadcaster<R>::Register(static_cast<ChannelReceiver<R>*>(receiver));
			}

			if constexpr (sizeof...(Rs) >= 1)
			{
				ChannelBroadcaster<Ts...>::Register(static_cast<ChannelReceiver<Rs...>*>(receiver));
			}
		}


		template<typename R, typename... Rs>
		void Unregister(ChannelReceiver<R, Rs...>* receiver)
		{
			if constexpr (std::is_same_v<R, T> || (std::is_same_v<R, Ts> || ...))
			{
				ChannelBroadcaster<R>::Unregister(static_cast<ChannelReceiver<R>*>(receiver));
			}

			if constexpr (sizeof...(Rs) >= 1)
			{
				ChannelBroadcaster<Ts...>::Unregister(static_cast<ChannelReceiver<Rs...>*>(receiver));
			}
		}
	};


	template<std::copyable T>
	class ChannelBroadcaster<T>
	{
		template<std::copyable, std::copyable...>
		friend
		class ChannelBroadcaster;

	public:
		ChannelBroadcaster() = default;

		ChannelBroadcaster(const ChannelBroadcaster& rhs)            = delete;
		ChannelBroadcaster& operator=(const ChannelBroadcaster& rhs) = delete;
		ChannelBroadcaster(ChannelBroadcaster&& rhs)                 = default;
		ChannelBroadcaster& operator=(ChannelBroadcaster&& rhs)      = delete;


		void Register(ChannelReceiver<T>* receiver)
		{
			mReceivers.emplace(receiver->GetReflexivePointer());
		}


		void Unregister(ChannelReceiver<T>* receiver)
		{
			mReceivers.erase(receiver->GetReflexivePointer());
		}


		void Broadcast(T value)
		{
			std::set<ReflexivePointer<ChannelReceiver<T>>> toRemove;

			for (auto& receiver: mReceivers)
			{
				if (receiver) receiver->Receive(value);
				else
					toRemove.emplace(receiver);
			}

			for (auto& receiver: toRemove)
			{
				mReceivers.erase(receiver);
			}
		}

	protected:
		std::set<Core::ReflexivePointer<ChannelReceiver<T>>> mReceivers;
	};
} // namespace Strawberry::Core::IO

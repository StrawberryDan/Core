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
	template <std::copyable T, std::copyable... Ts>
	class ChannelBroadcaster
		: private ChannelBroadcaster<T>
		, private ChannelBroadcaster<Ts...>
	{
	public:
		using ChannelBroadcaster<T>::Broadcast;
		using ChannelBroadcaster<Ts>::Broadcast...;

		template <typename R, typename... Rs>
		void Register(ChannelReceiver<R, Rs...>* receiver)
		{
			ChannelBroadcaster<R>::Register(static_cast<ChannelReceiver<R>*>(receiver));
			ChannelBroadcaster<Rs...>::Register(static_cast<ChannelReceiver<Rs...>*>(receiver));
		}
	};

	template <std::copyable T>
	class ChannelBroadcaster<T>
	{
		template <std::copyable, std::copyable...>
		friend class ChannelBroadcaster;

	public:
		ChannelBroadcaster()                                         = default;

		ChannelBroadcaster(const ChannelBroadcaster& rhs)            = delete;
		ChannelBroadcaster& operator=(const ChannelBroadcaster& rhs) = delete;
		ChannelBroadcaster(ChannelBroadcaster&& rhs)                 = default;
		ChannelBroadcaster& operator=(ChannelBroadcaster&& rhs)      = delete;

		void Register(ChannelReceiver<T>* receiver) { mReceivers.emplace(receiver->mManagedThis); }

		void Broadcast(T value)
		{
			for (auto& receiver : mReceivers)
			{
				auto lock = receiver->Lock();
				if (*lock) { (*lock)->Receive(value); }
			}
		}


	protected:
		std::set<std::shared_ptr<Mutex<ChannelReceiver<T>*>>> mReceivers;
	};
} // namespace Strawberry::Core::IO
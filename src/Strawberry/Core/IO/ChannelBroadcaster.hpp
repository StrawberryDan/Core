#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Core/IO/ChannelReceiver.hpp"
#include "Strawberry/Core/Types/TypeSet.hpp"
// Standard Library
#include <set>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template <typename... Ts>
	class ChannelBroadcaster;


	template<typename... Ts>
	class ChannelBroadcaster
			: ChannelBroadcaster<Ts>...
	{
	public:
		template <typename... Args>
		void Register(ChannelReceiver<Args...>& receiver)
		{
			using Intersection = typename TypeSet<Ts...>::template Intersection<Args...>;
			using RegisterFN = void(*)(void*, void*);

			RegisterFN functions[] = {
				[](void* pReceiver, void* pBroadcaster) constexpr -> void
				{
					if constexpr(Intersection::template Contains<Args>)
					{
						ChannelReceiver<Args>& receiver = *static_cast<ChannelReceiver<Args...>*>(pReceiver);
						ChannelBroadcaster<Args>& broadcaster = *static_cast<ChannelBroadcaster*>(pBroadcaster);
						broadcaster.Register(receiver);
					}
				}...
			};

			for (auto&& f : functions)
			{
				std::invoke(f, &receiver, this);
			}
		}


		template <typename... Args>
		void Unregister(ChannelReceiver<Args...>& receiver)
		{
			using Intersection = typename TypeSet<Ts...>::template Intersection<Args...>;
			using RegisterFN = void(*)(void*, void*);

			RegisterFN functions[] = {
				[](void* pReceiver, void* pBroadcaster) constexpr -> void
				{
					if constexpr(Intersection::template Contains<Args>)
					{
						ChannelReceiver<Args>& receiver = *static_cast<ChannelReceiver<Args...>*>(pReceiver);
						ChannelBroadcaster<Args>& broadcaster = *static_cast<ChannelBroadcaster*>(pBroadcaster);
						broadcaster.Register(receiver);
					}
				}...
			};

			for (auto&& f : functions)
			{
				std::invoke(f, &receiver, this);
			}
		}


		using ChannelBroadcaster<Ts>::Broadcast...;
	};


	template<typename T>
	class ChannelBroadcaster<T>
	{
		template<typename...>
		friend class ChannelBroadcaster;

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


	protected:
		void Broadcast(const T& value)
		{
			std::set<ReflexivePointer<ChannelReceiver<T>>> receivers;

			for (auto& receiver: mReceivers)
			{
				if (receiver)
				{
					receiver->Receive(value);
					receivers.emplace(receiver);;
				}

			}

			mReceivers = std::move(receivers);
		}

	private:
		std::set<ReflexivePointer<ChannelReceiver<T>>> mReceivers;
	};
} // namespace Strawberry::Core::IO

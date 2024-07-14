#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Core/Sync/Mutex.hpp"
#include "Strawberry/Core/Types/ReflexivePointer.hpp"
// Standard Library
#include <set>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template<std::copyable T, std::copyable... Ts>
	class ChannelBroadcaster;


	template<std::copyable T, std::copyable... Ts>
	class ChannelReceiver
			: protected ChannelReceiver<T>, protected ChannelReceiver<Ts...>
	{
		template<std::copyable, std::copyable...>
		friend class ChannelBroadcaster;

		protected:
			using ChannelReceiver<T>::Receive;
			using ChannelReceiver<Ts>::Receive...;
	};


	template<std::copyable T>
	class ChannelReceiver<T> : public EnableReflexivePointer
	{
		template<std::copyable, std::copyable...>
		friend class ChannelBroadcaster;

		public:
			ChannelReceiver() {}

			ChannelReceiver(const ChannelReceiver& rhs)            = delete;
			ChannelReceiver& operator=(const ChannelReceiver& rhs) = delete;


			ChannelReceiver(ChannelReceiver&& rhs)
			{
				*rhs.mManagedThis = this;
			}


			ChannelReceiver& operator=(ChannelReceiver&& rhs) = delete;

			virtual void Receive(T value) = 0;
	};
} // namespace Strawberry::Core::IO

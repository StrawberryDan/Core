#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Sync/Mutex.hpp"
// Standard Library
#include <set>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template <std::copyable T, std::copyable... Ts>
	class ChannelBroadcaster;

	template <std::copyable T, std::copyable... Ts>
	class ChannelReceiver
		: protected ChannelReceiver<T>
		, protected ChannelReceiver<Ts...>
	{
		template <std::copyable, std::copyable...>
		friend class ChannelBroadcaster;

	protected:
		using ChannelReceiver<T>::Receive;
		using ChannelReceiver<Ts>::Receive...;
	};

	template <std::copyable T>
	class ChannelReceiver<T>
	{
		template <std::copyable, std::copyable...>
		friend class ChannelBroadcaster;


	public:
		ChannelReceiver()
			: mManagedThis(std::make_shared<Core::Mutex<ChannelReceiver*>>(this))
		{}

		ChannelReceiver(const ChannelReceiver& rhs)            = delete;
		ChannelReceiver& operator=(const ChannelReceiver& rhs) = delete;

		ChannelReceiver(ChannelReceiver&& rhs) { *rhs.mManagedThis = this; }

		ChannelReceiver& operator=(ChannelReceiver&& rhs) = delete;

		~ChannelReceiver() { (*mManagedThis->Lock()) = nullptr; }

		virtual void Receive(T value){};

	private:
		std::shared_ptr<Core::Mutex<ChannelReceiver*>> mManagedThis;
	};
} // namespace Strawberry::Core::IO

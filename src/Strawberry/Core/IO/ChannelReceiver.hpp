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
	template<typename T, typename... Ts>
	class ChannelBroadcaster;


	template<typename T, typename... Ts>
	class ChannelReceiver
			: public ChannelReceiver<T>, public ChannelReceiver<Ts...>
	{
		template<typename, typename...>
		friend class ChannelBroadcaster;

		virtual ~ChannelReceiver() = default;

	protected:
		using ChannelReceiver<T>::Receive;
		using ChannelReceiver<Ts>::Receive...;
	};


	template<typename T>
	class ChannelReceiver<T> : public EnableReflexivePointer
	{
		template<typename, typename...>
		friend class ChannelBroadcaster;

	public:
		ChannelReceiver() = default;
		virtual ~ChannelReceiver() {};

		ChannelReceiver(const ChannelReceiver& rhs)            = delete;
		ChannelReceiver& operator=(const ChannelReceiver& rhs) = delete;


		ChannelReceiver(ChannelReceiver&&) = default;
		ChannelReceiver& operator=(ChannelReceiver&&) = default;

		virtual void Receive(const T& value) = 0;
	};
} // namespace Strawberry::Core::IO

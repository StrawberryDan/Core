#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Core/Types/ReflexivePointer.hpp"

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template<typename... Ts>
	class ChannelBroadcaster;


	template<typename... Ts>
	class ChannelReceiver
			: ChannelReceiver<Ts>...
	{
	public:
		template<typename...>
		friend class ChannelBroadcaster;

		virtual ~ChannelReceiver() noexcept = default;

	protected:
		using ChannelReceiver<Ts>::Receive...;
	};


	template<typename T>
	class ChannelReceiver<T> : public EnableReflexivePointer
	{
		template<typename...>
		friend class ChannelBroadcaster;

	public:
		ChannelReceiver() noexcept = default;
		virtual ~ChannelReceiver() noexcept = default;

		ChannelReceiver(const ChannelReceiver& rhs)            = delete;
		ChannelReceiver& operator=(const ChannelReceiver& rhs) = delete;


		ChannelReceiver(ChannelReceiver&&) noexcept = default;
		ChannelReceiver& operator=(ChannelReceiver&&) noexcept = default;


	protected:
		virtual void Receive(const T& value) = 0;
	};
} // namespace Strawberry::Core::IO

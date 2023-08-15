#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Sync/Mutex.hpp"
// Standard Library
#include <deque>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template <typename T>
	class ChannelBroadcaster;


	template <typename T>
	class ChannelReceiver
	{
		friend class ChannelBroadcaster<T>;


	public:
		ChannelReceiver(const ChannelReceiver& rhs)            = default;
		ChannelReceiver& operator=(const ChannelReceiver& rhs) = default;
		ChannelReceiver(ChannelReceiver&& rhs)                 = default;
		ChannelReceiver& operator=(ChannelReceiver&& rhs)      = default;


		~ChannelReceiver()
		{
			mMessageBuffer.Lock()->clear();
		}


		Core::Option<T> Read()
		{
			auto messageBuffer = mMessageBuffer.Lock();
			auto result        = messageBuffer->empty() ? Core::NullOpt : Core::Option(messageBuffer->front());
			if (result)
			{
				messageBuffer->pop_front();
			}
			return result;
		}


	protected:
		ChannelReceiver() = default;


		void Receive(T value)
		{
			mMessageBuffer.Lock()->push_back(std::move(value));
		}


	private:
		Core::Mutex<std::deque<T>> mMessageBuffer;
	};
} // namespace Strawberry::Core::IO

#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "ChannelReceiver.hpp"


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template<std::copyable T>
	class CallbackChannelReceiver
			: public IO::ChannelReceiver<T>
	{
	public:
		CallbackChannelReceiver() = default;


		CallbackChannelReceiver(std::function<void(T)> callback)
			: mCallback(std::move(callback)) {}


		CallbackChannelReceiver(const CallbackChannelReceiver& rhs)
			: IO::ChannelReceiver<T>()
			, mCallback(rhs.mCallback) {}


		CallbackChannelReceiver& operator=(const CallbackChannelReceiver& rhs)
		{
			if (this != &rhs)
			{
				mCallback = rhs.mCallback;
			}

			return *this;
		}


		CallbackChannelReceiver(CallbackChannelReceiver&& rhs) noexcept
			: IO::ChannelReceiver<T>(std::move(rhs))
		{
			mCallback = std::move(rhs.mCallback);
		}


		CallbackChannelReceiver& operator=(CallbackChannelReceiver&& rhs)
		{
			if (this != &rhs)
			{
				static_cast<IO::ChannelReceiver<T>&>(*this) = std::move(static_cast<IO::ChannelReceiver<T>&>(rhs));
				mCallback                                   = std::move(rhs.mCallback);
			}

			return *this;
		}

	protected:
		virtual void Receive(T value) override final
		{
			if (mCallback) mCallback(value);
		}

	private:
		std::function<void(T)> mCallback;
	};
}

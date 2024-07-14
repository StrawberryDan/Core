#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/IO/Receiver.hpp"
// Standard Library
#include <functional>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template<std::copyable T>
	class CallbackReceiver : public Receiver<T>
	{
		public:
			CallbackReceiver() = default;


			CallbackReceiver(std::function<void(T)> callback)
				: mCallback(std::move(callback)) {}


			CallbackReceiver(const CallbackReceiver& rhs)
				: Receiver<T>()
				, mCallback(rhs.mCallback) {}


			CallbackReceiver& operator=(const CallbackReceiver& rhs)
			{
				if (this != &rhs)
				{
					mCallback = rhs.mCallback;
				}

				return *this;
			}


			CallbackReceiver(CallbackReceiver&& rhs)
				: Receiver<T>(std::move(rhs))
			{
				mCallback = std::move(rhs.mCallback);
			}


			CallbackReceiver& operator=(CallbackReceiver&& rhs)
			{
				if (this != &rhs)
				{
					static_cast<Receiver<T>&>(*this) = std::move(static_cast<Receiver<T>&>(rhs));
					mCallback                        = std::move(rhs.mCallback);
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
} // namespace Strawberry::Core::IO

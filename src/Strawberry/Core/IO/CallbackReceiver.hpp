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
	template<typename T>
	class CallbackReceiver : public Receiver<T>
	{
	public:
		CallbackReceiver() = default;


		CallbackReceiver(std::function<void(const T&)> callback)
			: mCallback(std::move(callback)) {}


		CallbackReceiver(const CallbackReceiver& rhs) = delete;
		CallbackReceiver& operator=(const CallbackReceiver& rhs) = delete;


		CallbackReceiver(CallbackReceiver&& rhs) = default;
		CallbackReceiver& operator=(CallbackReceiver&& rhs) = default;

	protected:
		virtual void Receive(const T& value) override final
		{
			if (mCallback) mCallback(value);
		}

	private:
		std::function<void(T&)> mCallback;
	};
} // namespace Strawberry::Core::IO

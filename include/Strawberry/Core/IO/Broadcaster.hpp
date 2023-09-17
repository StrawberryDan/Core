#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/IO/Receiver.hpp"
#include "Strawberry/Core/Sync/Mutex.hpp"
// Standard Library
#include <set>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template <std::copyable... Ts>
	class Broadcaster : private Broadcaster<Ts>...
	{
	public:
		using Broadcaster<Ts>::Broadcast...;
	};

	template <std::copyable T>
	class Broadcaster<T>
	{
	protected:
		void Broadcast(T value)
		{
			auto receivers = Receiver<T>::sReceivers.Lock();
			for (auto& receiver : *receivers) { receiver->Receive(value); }
		}
	};
} // namespace Strawberry::Core::IO
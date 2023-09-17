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
	template <std::copyable T>
	class Broadcaster
	{
	protected:
		void Broadcast(T value)
		{
			auto receivers = Receiver<T>::sReceivers.Lock();
			for (auto& receiver : *receivers) { receiver->Receive(value); }
		}
	};
} // namespace Strawberry::Core::IO
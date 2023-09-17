#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Sync/Mutex.hpp"
// Standard Library
#include <set>

//======================================================================================================================
//  Foreward Declarations
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template <std::copyable T>
	class Broadcaster;
}

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template <std::copyable T>
	class Receiver
	{
		friend class Broadcaster<T>;

	public:
		Receiver()
		{
			auto receiverList = sReceivers.Lock();
			receiverList->emplace(this);
		}

		Receiver(const Receiver& rhs)            = delete;

		Receiver& operator=(const Receiver& rhs) = delete;

		Receiver(Receiver&& rhs)
		{
			auto receiverList = sReceivers.Lock();
			receiverList->erase(&rhs);
			receiverList->emplace(this);
		}

		Receiver& operator=(Receiver&& rhs)
		{
			if (this != &rhs)
			{
				auto receiverList = sReceivers.Lock();
				receiverList->erase(&rhs);
				receiverList->emplace(this);
			}
			return *this;
		}

		~Receiver()
		{
			auto receiverList = sReceivers.Lock();
			receiverList->erase(this);
		}

	protected:
		virtual void Receive(T value) = 0;


	private:
		static Core::Mutex<std::set<Receiver<T>*>> sReceivers;
	};

	template <std::copyable T>
	Core::Mutex<std::set<Receiver<T>*>> Receiver<T>::sReceivers;
} // namespace Strawberry::Core::IO
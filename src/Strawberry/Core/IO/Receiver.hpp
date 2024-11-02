#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Sync/Mutex.hpp"
// Standard Library
#include <set>

#include "Strawberry/Core/Types/ReflexivePointer.hpp"

//======================================================================================================================
//  Foreward Declarations
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template<typename, typename...>
	class Broadcaster;
}

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::IO
{
	template<typename T, typename... Ts>
	class Receiver
			: private Receiver<T>, private Receiver<Ts...>
	{
	protected:
		using Receiver<T>::Receive;
		using Receiver<Ts>::Receive...;
	};


	template<typename T>
	class Receiver<T>
		: public EnableReflexivePointer
	{
		template<typename, typename...>
		friend class Broadcaster;

	public:
		Receiver()
		{
			auto receiverList = sReceivers.Lock();
			receiverList->emplace(GetReflexivePointer());
		}


		Receiver(const Receiver& rhs) = delete;
		Receiver& operator=(const Receiver& rhs) = delete;
		Receiver(Receiver&& rhs) = default;
		Receiver& operator=(Receiver&& rhs) = default;


		~Receiver()
		{
			auto receiverList = sReceivers.Lock();
			receiverList->erase(GetReflexivePointer());
		}

	protected:
		virtual void Receive(const T& value) = 0;

	private:
		inline static Core::Mutex<std::set<ReflexivePointer<Receiver<T>>>> sReceivers;
	};

} // namespace Strawberry::Core::IO

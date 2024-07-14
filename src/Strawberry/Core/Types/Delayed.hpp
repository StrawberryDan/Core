#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <memory>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template<typename T>
	class Delayed
	{
	public:
		Delayed() {}


		Delayed(const Delayed& rhs) = delete;


		Delayed& operator=(const Delayed& rhs) = delete;


		Delayed(Delayed&&) = delete;


		Delayed operator=(Delayed&&) = delete;


		~Delayed()
		{
			std::destroy_at(&mPayload);
		}


		template<typename... Args> requires std::constructible_from<T, Args...>
		void Construct(Args... args)
		{
			std::construct_at(&mPayload, std::forward<Args>(args)...);
		}


		void Destruct()
		{
			std::destroy_at(&mPayload);
		}


		T& Get()
		{
			return mPayload;
		}


		const T& Get() const
		{
			return mPayload;
		}


		T& operator*()
		{
			return mPayload;
		}


		const T& operator*() const
		{
			return mPayload;
		}


		T* operator->()
		{
			return &mPayload;
		}


		const T* operator->() const
		{
			return &mPayload;
		}

	private:
		union
		{
			T mPayload;
		};
	};
}

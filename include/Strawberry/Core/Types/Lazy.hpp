#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Core/Util/Flag.hpp"
// Standard Library
#include <concepts>
#include <functional>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template <typename T>
	class Lazy
	{
	public:
		T* Get()
		{
			if (!mInitialised)
			{
				mPayload = mInitialiser();
				mInitialised.Set();
			}

			return &mPayload;
		}


	protected:
		Lazy()
			requires (std::default_initializable<T>)
			: mInitialiser([]() -> T { return T(); })
		{}

		template <typename F>
			requires requires { F()->T; }
		explicit Lazy(F f)
			: mInitialiser(std::move(f))
		{}


	protected:
		Flag mInitialised;

		union
		{
			T                  mPayload;
			std::function<T()> mInitialiser;
		};
	};
} // namespace Strawberry::Core
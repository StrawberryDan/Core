#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <concepts>
#include <functional>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template <std::copyable T>
	class DynamicValue
	{
	public:
		DynamicValue(std::function<T()> initialiser)
			: mValue()
			  , mInitialiser(std::move(initialiser))
		{}


		DynamicValue(const DynamicValue& rhs) = default;
		DynamicValue& operator=(const DynamicValue& rhs) = default;
		DynamicValue(DynamicValue&& rhs) noexcept = delete;
		DynamicValue& operator=(DynamicValue&& rhs) noexcept = delete;


		T Get()
		{
			if (!mValue)
			{
				mValue = mInitialiser();
			}
			return mValue.Value();
		}


		void Invalidate()
		{
			mValue.Reset();
		}


	private:
		Optional<T> mValue;
		std::function<T()> mInitialiser;
	};
}

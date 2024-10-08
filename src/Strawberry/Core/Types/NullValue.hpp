#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <concepts>
#include <limits>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template<std::integral T, T Null>
	class NullValue
	{
	public:
		using Inner = T;


		static constexpr T NULL_VALUE = Null;

		NullValue() = default;


		NullValue(T value)
			: mValue(value) {}


		explicit operator bool() const noexcept
		{
			return mValue != Null;
		}


		operator T() const noexcept
		{
			return mValue;
		}


		NullValue& operator++()
		{
			return mValue++;
		}


		NullValue& operator++(int)
		{
			return ++mValue;
		}


		NullValue& operator--()
		{
			return mValue--;
		}


		NullValue& operator--(int)
		{
			return --mValue;
		}


	private:
		T mValue = Null;
	};


	template<std::integral T>
	using NonZero = NullValue<T, T{0}>;

	template<std::integral T>
	using NonMax = NullValue<T, std::numeric_limits<T>::max()>;
}

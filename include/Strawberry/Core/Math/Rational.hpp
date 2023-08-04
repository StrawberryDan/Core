#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Standard Library
#include <concepts>


namespace Strawberry::Core::Math
{
	template <std::integral T>
	class Rational
	{
	public:
		Rational(T numerator, T denominator) : mNumerator(numerator), mDenominator(denominator) {}


		T&       Numerator()         { return mNumerator; }
		const T& Numerator() const   { return mNumerator; }
		T&       Denominator()       { return mDenominator; }
		const T& Denominator() const { return mDenominator; }


		double Evaluate() const { return static_cast<double>(mNumerator) / static_cast<double>(mDenominator); }
		double operator*() const { return Evaluate(); }


	private:
		T mNumerator;
		T mDenominator;
	};
}
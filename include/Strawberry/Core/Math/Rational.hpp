#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Math.hpp"
// Standard Library
#include <concepts>
#include <cstdint>


namespace Strawberry::Core::Math
{
	template <std::integral T = int32_t>
	class Rational
	{
	public:
		Rational(T value) : mNumerator(value), mDenominator(1) {}
		Rational(T numerator, T denominator) : mNumerator(numerator), mDenominator(denominator) { Normalize(); }


		void SetNumerator(T value)   { mNumerator = value; Normalize(); }
		void SetDenominator(T value) { mDenominator = value; Normalize(); }

		
		const T& Numerator() const   { return mNumerator; }
		const T& Denominator() const { return mDenominator; }


		double Evaluate() const { return static_cast<double>(mNumerator) / static_cast<double>(mDenominator); }
		double operator*() const { return Evaluate(); }


		Rational operator+(const Rational& rhs) const
			{ return {Numerator() * rhs.Denominator() + rhs.Numerator() * Denominator(), Denominator() * rhs.Denominator() }; }
		Rational operator-(const Rational& rhs) const
			{ return {Numerator() * rhs.Denominator() - rhs.Numerator() * Denominator(), Denominator() * rhs.Denominator() }; }
		Rational operator*(const Rational& rhs) const
			{ return {Numerator() * rhs.Numerator(), Denominator() * rhs.Denominator()}; }
		Rational operator/(const Rational& rhs) const
			{ return {Numerator() * rhs.Denominator(), Denominator() * rhs.Numerator()}; }


	protected:
		void Normalize()
		{
			for (auto gcd = GreatestCommonDivisor(mNumerator, mDenominator);
				 gcd != 1;
				 gcd = GreatestCommonDivisor(mNumerator, mDenominator))
			{
				mNumerator /= gcd;
				mDenominator /= gcd;
			}
		}


	private:
		T mNumerator;
		T mDenominator;
	};


	template <std::integral T>
	Rational(T, T) -> Rational<T>;
}
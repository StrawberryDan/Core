#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Standard Library
#include <numbers>


//======================================================================================================================
//  Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Math
{
	class Radians
	{
	public:
		constexpr Radians() : mValue(0.0) {}
		constexpr Radians(double value) : mValue(value) {}
		constexpr operator double() const { return mValue; }

	private:
		double mValue;
	};


	class Degrees
	{
	public:
		constexpr Degrees() : mValue(0.0) {}
		constexpr Degrees(double value) : mValue(value) {}
		constexpr Degrees(Radians radians) : mValue(radians * (180.0 / std::numbers::pi)) {}
		constexpr operator double() const { return mValue; }
		constexpr operator Radians() const { return mValue * (std::numbers::pi / 180.0); }

	private:
		double mValue;
	};
} // namespace Strawberry::Core::Math

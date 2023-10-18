#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------


//======================================================================================================================
//  Definitions
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Math
{
	class Radians
	{
	public:
		constexpr Radians();
		constexpr Radians(double value);
		constexpr operator double() const;

	private:
		double mValue;
	};


	class Degrees
	{
	public:
		constexpr Degrees();
		constexpr Degrees(double value);
		constexpr Degrees(Radians radians);
		constexpr operator double() const;
		constexpr operator Radians() const;

	private:
		double mValue;
	};
} // namespace Strawberry::Core::Math
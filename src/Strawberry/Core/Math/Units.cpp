//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Math/Units.hpp"
// Standard Library
#include <numbers>

namespace Strawberry::Core::Math
{
	constexpr Radians::Radians()
		: mValue(0.0)
	{}

	constexpr Radians::Radians(double value)
		: mValue(value)
	{}

	constexpr Radians::operator double() const
	{
		return mValue;
	}

	constexpr Degrees::Degrees()
		: mValue(0.0)
	{}

	constexpr Degrees::Degrees(double value)
		: mValue(value)
	{}

	constexpr Degrees::Degrees(Radians radians)
		: mValue(radians * (180.0 / std::numbers::pi))
	{}

	constexpr Degrees::operator double() const
	{
		return mValue;
	}

	constexpr Degrees::operator Radians() const
	{
		return mValue * (std::numbers::pi / 180.0);
	}
} // namespace Strawberry::Core::Math
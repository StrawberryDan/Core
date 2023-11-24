#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include <concepts>
#include <cmath>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Math
{
	template<typename T> requires std::integral<T> || std::floating_point<T>
	class Clamped
	{
	public:
		Clamped(T min, T max, T value = T{})
			: mMin(min)
			, mMax(max)
			, mValue(std::clamp<T>(value, mMin, mMax))
		{}


		explicit operator T() const { return mValue; }


		auto operator<=>(const Clamped&) const = default;


		bool operator==(T rhs) const
		{
			return mValue == rhs;
		}


		bool operator!=(T rhs) const
		{
			return mValue != rhs;
		}


		bool operator<(T rhs) const
		{
			return mValue < rhs;
		}


		bool operator>(T rhs) const
		{
			return mValue > rhs;
		}


		bool operator<=(T rhs) const
		{
			return mValue <= rhs;
		}


		bool operator>=(T rhs) const
		{
			return mValue >= rhs;
		}


		Clamped operator+(const T& rhs) const
		{
			return Clamped(mMin, mMax, mValue + rhs);
		}


		Clamped operator-(const T& rhs) const
		{
			return Clamped(mMin, mMax, (mValue - rhs));
		}


		Clamped operator*(const T& rhs) const
		{
			return Clamped((mMin, mMax, mValue * rhs));
		}


		Clamped operator/(const T& rhs) const
		{
			return Clamped((mMin, mMax, mValue / rhs));
		}


		Clamped& operator+=(const T& rhs)
		{
			mValue += rhs;
			mValue = std::clamp<T>(mValue, mMin, mMax);
			return *this;
		}


		Clamped& operator-=(const T& rhs)
		{
			mValue -= rhs;
			mValue = std::clamp<T>(mValue, mMin, mMax);
			return *this;
		}


		Clamped& operator*=(const T& rhs)
		{
			mValue *= rhs;
			mValue = std::clamp<T>(mValue, mMin, mMax);
			return *this;
		}


		Clamped& operator/=(const T& rhs)
		{
			mValue /= rhs;
			mValue = std::clamp<T>(mValue, mMin, mMax);
			return *this;
		}


		Clamped operator+(const Clamped& rhs) const
		{
			return Clamped(mMin, mMax, mValue + rhs.mValue);
		}


		Clamped operator-(const Clamped& rhs) const
		{
			return Clamped(mMin, mMax, mValue - rhs.mValue);
		}


		Clamped operator*(const Clamped& rhs) const
		{
			return Clamped(mMin, mMax, mValue * rhs.mValue);
		}


		Clamped operator/(const Clamped& rhs) const
		{
			return Clamped(mMin, mMax, mValue / rhs.mValue);
		}


		Clamped& operator+=(const Clamped& rhs)
		{
			mValue += rhs;
			mValue = std::clamp<T>(mValue, mMin, mMax);
			return *this;
		}


		Clamped& operator-=(const Clamped& rhs)
		{
			mValue -= rhs;
			mValue = std::clamp<T>(mValue, mMin, mMax);
			return *this;
		}


		Clamped& operator*=(const Clamped& rhs)
		{
			mValue *= rhs;
			mValue = std::clamp<T>(mValue, mMin, mMax);
			return *this;
		}


		Clamped& operator/=(const Clamped& rhs)
		{
			mValue /= rhs;
			mValue = std::clamp<T>(mValue, mMin, mMax);
			return *this;
		}


	private:
		T mMin;
		T mMax;
		T mValue;
	};
}

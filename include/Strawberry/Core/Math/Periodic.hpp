#pragma once


#include "Strawberry/Core/Util/Logging.hpp"
#include <concepts>


namespace Strawberry::Core::Math
{
	//======================================================================================================================
	//  Forward Declarations
	//----------------------------------------------------------------------------------------------------------------------
	template <typename T>
	class DynamicPeriodic;


	//======================================================================================================================
	//  Class Definitions
	//----------------------------------------------------------------------------------------------------------------------
	//  Static Periodic Integer
	//----------------------------------------------------------------------------------------------------------------------
	template <typename T, T N>
	requires std::unsigned_integral<T>
	class Periodic
	{
	public:
		Periodic()
			: mValue{}
		{
		}


		Periodic(T value)
			: mValue(value % N)
		{
		}


		auto operator<=>(const Periodic& rhs) const = default;


		Periodic operator-() { return N - mValue; }


		Periodic operator++()
		{
			*this += 1;
			return *this;
		}


		Periodic operator--()
		{
			*this -= 1;
			return *this;
		}


		Periodic operator++(int)
		{
			Periodic val  = *this;
			*this        += 1;
			return val;
		}


		Periodic operator--(int)
		{
			Periodic val  = *this;
			*this        -= 1;
			return val;
		}


		Periodic operator+(Periodic rhs) const { return (mValue + rhs.mValue) % N; }


		Periodic operator-(Periodic rhs) const
		{
			return rhs.mValue >= mValue ? (N - (rhs.mValue - mValue)) % N : (mValue - rhs.mValue) % N;
		}


		Periodic operator*(Periodic rhs) const { return (mValue * rhs.mValue) % N; }


		Periodic operator/(Periodic rhs) const { return (mValue / rhs.mValue) % N; }


		void operator+=(Periodic rhs) { *this = *this + rhs; }


		void operator-=(Periodic rhs) { *this = *this - rhs; }


		void operator*=(Periodic rhs) { *this = *this * rhs; }


		void operator/=(Periodic rhs) { *this = *this / rhs; }


		explicit operator T() const { return mValue; }


		auto operator*() const { return mValue; }


		auto Value() const { return mValue(); }


		explicit operator DynamicPeriodic<T>() const { return DynamicPeriodic<T>(N, mValue); }


	private:
		T mValue;
	};


	//======================================================================================================================
	//  Dynamic Periodic Number
	//----------------------------------------------------------------------------------------------------------------------
	//  Base Case : Do not use
	//----------------------------------------------------------------------------------------------------------------------
	template <typename T>
	class DynamicPeriodic
	{
	public:
		DynamicPeriodic()  = delete;
		~DynamicPeriodic() = delete;
	};


	//======================================================================================================================
	//  Unsigned Integer Periodic Number
	//----------------------------------------------------------------------------------------------------------------------
	template <typename T>
	requires std::unsigned_integral<T>
	class DynamicPeriodic<T>
	{
	public:
		DynamicPeriodic(T value)
			: mMax(std::numeric_limits<T>::max())
			, mValue(value % mMax)
		{
		}


		explicit DynamicPeriodic(T max, T value)
			: mMax(max)
			, mValue(value)
		{
		}


		T GetMax() const { return mMax; }


		void SetMax(T max)
		{
			mMax   = max;
			mValue = mValue % mMax;
		}


		auto operator<=>(const DynamicPeriodic& rhs) const { return mValue <=> rhs.mValue; }


		bool operator==(const DynamicPeriodic& rhs) const { return mValue == rhs.mValue; }


		bool operator!=(const DynamicPeriodic& rhs) const { return mValue != rhs.mValue; }


		bool operator>(const DynamicPeriodic& rhs) const { return mValue > rhs.mValue; }


		bool operator>=(const DynamicPeriodic& rhs) const { return mValue >= rhs.mValue; }


		bool operator<(const DynamicPeriodic& rhs) const { return mValue < rhs.mValue; }


		bool operator<=(const DynamicPeriodic& rhs) const { return mValue <= rhs.mValue; }


		DynamicPeriodic operator-() { return mMax - mValue; }


		DynamicPeriodic operator++()
		{
			*this += 1;
			return *this;
		}


		DynamicPeriodic operator--()
		{
			*this -= 1;
			return *this;
		}


		DynamicPeriodic operator++(int)
		{
			DynamicPeriodic val(*this);
			*this += DynamicPeriodic(mMax, 1);
			return val;
		}


		DynamicPeriodic operator--(int)
		{
			DynamicPeriodic val(*this);
			*this -= DynamicPeriodic(mMax, 1);
			return val;
		}


		DynamicPeriodic operator+(DynamicPeriodic rhs) const
		{
			rhs.SetMax(mMax);
			return DynamicPeriodic(mMax, (mValue + rhs.mValue) % mMax);
		}


		DynamicPeriodic operator-(DynamicPeriodic rhs) const
		{
			rhs.SetMax(mMax);
			return rhs.mValue >= mValue ? DynamicPeriodic(mMax, (mMax - (rhs.mValue - mValue)) % mMax)
										: DynamicPeriodic(mMax, (mValue - rhs.mValue) % mMax);
		}


		DynamicPeriodic operator*(DynamicPeriodic rhs) const
		{
			rhs.SetMax(mMax);
			return DynamicPeriodic(mMax, (mValue * rhs.mValue) % mMax);
		}


		DynamicPeriodic operator/(DynamicPeriodic rhs) const
		{
			rhs.SetMax(mMax);
			return DynamicPeriodic(mMax, (mValue / rhs.mValue) % mMax);
		}


		void operator+=(DynamicPeriodic rhs) { *this = *this + rhs; }


		void operator-=(DynamicPeriodic rhs) { *this = *this - rhs; }


		void operator*=(DynamicPeriodic rhs) { *this = *this * rhs; }


		void operator/=(DynamicPeriodic rhs) { *this = *this / rhs; }


		explicit operator T() const { return mValue; }


		T operator*() const { return mValue; }


		T Value() const { return mValue; }


	private:
		T mMax;
		T mValue;
	};


	//======================================================================================================================
	//  Floating Point Periodic Number
	//----------------------------------------------------------------------------------------------------------------------
	template <typename T>
	requires std::floating_point<T>
	class DynamicPeriodic<T>
	{
	public:
		DynamicPeriodic(T value)
			: mMax(std::numeric_limits<T>::max())
			, mValue(value >= 0 ? std::fmod(value, mMax) : mMax - std::fmod(-value, mMax))
		{
		}


		explicit DynamicPeriodic(T max, T value = 0)
			: mMax(max)
			, mValue(value >= 0 ? std::fmod(value, mMax) : mMax - std::fmod(-value, mMax))
		{
		}


		T GetMax() const { return mMax; }


		void SetMax(T max)
		{
			mMax   = max;
			mValue = std::fmod(mValue, mMax);
		}


		auto operator<=>(const DynamicPeriodic& rhs) const = default;


		DynamicPeriodic operator-() { return mMax - mValue; }


		DynamicPeriodic operator+(DynamicPeriodic rhs) const
		{
			rhs.SetMax(mMax);
			return std::fmod(mValue + rhs.mValue, mMax);
		}


		DynamicPeriodic operator-(DynamicPeriodic rhs) const
		{
			rhs.SetMax(mMax);
			return rhs.mValue >= mValue ? std::fmod(mMax - (rhs.mValue - mValue), mMax) : std::fmod(mValue - rhs.mValue, mMax);
		}


		DynamicPeriodic operator*(DynamicPeriodic rhs) const
		{
			rhs.SetMax(mMax);
			return std::fmod(mValue * rhs.mValue, mMax);
		}


		DynamicPeriodic operator/(DynamicPeriodic rhs) const
		{
			rhs.SetMax(mMax);
			return std::fmod(mValue / rhs.mValue, mMax);
		}


		void operator+=(DynamicPeriodic rhs) { *this = *this + rhs; }


		void operator-=(DynamicPeriodic rhs) { *this = *this - rhs; }


		void operator*=(DynamicPeriodic rhs) { *this = *this * rhs; }


		void operator/=(DynamicPeriodic rhs) { *this = *this / rhs; }


		explicit operator T() const { return mValue; }


		T operator*() const { return mValue; }


		T Value() const { return mValue; }


	private:
		T mMax;
		T mValue;
	};


	template <typename T>
	DynamicPeriodic(T, T) -> DynamicPeriodic<T>;
}// namespace Strawberry::Core::Math

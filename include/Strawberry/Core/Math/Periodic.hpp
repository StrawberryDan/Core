#pragma once


#include <concepts>




namespace Strawberry::Core::Math
{
	//======================================================================================================================
	//  Foreward Declarations
	//----------------------------------------------------------------------------------------------------------------------
	template <typename T>
	class DynamicPeriodic;



	//======================================================================================================================
	//  Class Definitions
	//----------------------------------------------------------------------------------------------------------------------
	//  Static Periodic Integer
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T, T N> requires std::unsigned_integral<T>
	class Periodic
	{
	public:
		Periodic()
				: mValue{}
		{}

		Periodic(T value)
				: mValue(value % N)
		{}

		auto operator<=>(const Periodic& rhs) const = default;

		Periodic operator-() const { return N - mValue; }

		Periodic operator++() const { *this += 1; return *this; }
		Periodic operator--() const { *this -= 1; return *this; }
		Periodic operator++(int) const { Periodic val = *this; *this += 1; return val; }
		Periodic operator--(int) const { Periodic val = *this; *this -= 1; return val; }

		Periodic operator+(Periodic rhs) const { return (mValue + rhs.mValue) % N; }
		Periodic operator-(Periodic rhs) const { return rhs.mValue >= mValue ? (N - (rhs.mValue - mValue)) % N : (mValue - rhs.mValue) % N; }
		Periodic operator*(Periodic rhs) const { return (mValue * rhs.mValue) % N; }
		Periodic operator/(Periodic rhs) const { return (mValue / rhs.mValue) % N; }

		void     operator+=(Periodic rhs) const { *this = *this + rhs; }
		void     operator-=(Periodic rhs) const { *this = *this - rhs; }
		void     operator*=(Periodic rhs) const { *this = *this * rhs; }
		void     operator/=(Periodic rhs) const { *this = *this / rhs; }

		explicit operator T() const
		{ return mValue; }

		explicit operator DynamicPeriodic<T>() const
		{ return DynamicPeriodic<T>(N, mValue); }

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
	template<typename T> requires std::unsigned_integral<T>
	class DynamicPeriodic<T>
	{
	public:
		DynamicPeriodic(T value)
			: mMax(std::numeric_limits<T>::max())
			, mValue(value)
		{}

		explicit DynamicPeriodic(T max, T value = 0)
			: mMax(max)
			, mValue(value)
		{}

		T    GetMax() const { return mMax; }
		void SetMax(T max) { mMax = max; mValue = mValue % mMax; }

		auto operator<=>(const DynamicPeriodic& rhs) const = default;

		DynamicPeriodic operator-() const { return mMax - mValue; }

		DynamicPeriodic operator++() const { *this += 1; return *this; }
		DynamicPeriodic operator--() const { *this -= 1; return *this; }
		DynamicPeriodic operator++(int) const { Periodic val = *this; *this += 1; return val; }
		DynamicPeriodic operator--(int) const { Periodic val = *this; *this -= 1; return val; }

		DynamicPeriodic operator+(DynamicPeriodic rhs) const { rhs.SetMax(mMax); return (mValue + rhs.mValue) % mMax; }
		DynamicPeriodic operator-(DynamicPeriodic rhs) const { rhs.SetMax(mMax); return rhs.mValue >= mValue ? (mMax - (rhs.mValue - mValue)) % mMax : (mValue - rhs.mValue) % mMax; }
		DynamicPeriodic operator*(DynamicPeriodic rhs) const { rhs.SetMax(mMax); return (mValue * rhs.mValue) % mMax; }
		DynamicPeriodic operator/(DynamicPeriodic rhs) const { rhs.SetMax(mMax); return (mValue / rhs.mValue) % mMax; }

		void     operator+=(DynamicPeriodic rhs) const { *this = *this + rhs; }
		void     operator-=(DynamicPeriodic rhs) const { *this = *this - rhs; }
		void     operator*=(DynamicPeriodic rhs) const { *this = *this * rhs; }
		void     operator/=(DynamicPeriodic rhs) const { *this = *this / rhs; }

		explicit operator T() const
		{ return mValue; }

	private:
		T mMax;
		T mValue;
	};



	//======================================================================================================================
	//  Floating Point Periodic Number
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T> requires std::floating_point<T>
	class DynamicPeriodic<T>
	{
	public:
		DynamicPeriodic(T value)
				: mMax(std::numeric_limits<T>::max())
				, mValue(value >= 0 ? std::fmod(value, mMax) : mMax - std::fmod(-value, mMax))
		{}

		explicit DynamicPeriodic(T max, T value = 0)
				: mMax(max)
				, mValue(value >= 0 ? std::fmod(value, mMax) : mMax - std::fmod(-value, mMax))
		{}

		T    GetMax() const { return mMax; }
		void SetMax(T max) { mMax = max; mValue = std::fmod(mValue, mMax); }

		auto operator<=>(const DynamicPeriodic& rhs) const = default;

		DynamicPeriodic operator-() const { return mMax - mValue; }

		DynamicPeriodic operator+(DynamicPeriodic rhs) const { rhs.SetMax(mMax); return std::fmod(mValue + rhs.mValue, mMax); }
		DynamicPeriodic operator-(DynamicPeriodic rhs) const { rhs.SetMax(mMax); return rhs.mValue >= mValue ? std::fmod(mMax - (rhs.mValue - mValue), mMax) : std::fmod(mValue - rhs.mValue, mMax); }
		DynamicPeriodic operator*(DynamicPeriodic rhs) const { rhs.SetMax(mMax); return std::fmod(mValue * rhs.mValue, mMax); }
		DynamicPeriodic operator/(DynamicPeriodic rhs) const { rhs.SetMax(mMax); return std::fmod(mValue / rhs.mValue, mMax); }

		void     operator+=(DynamicPeriodic rhs) const { *this = *this + rhs; }
		void     operator-=(DynamicPeriodic rhs) const { *this = *this - rhs; }
		void     operator*=(DynamicPeriodic rhs) const { *this = *this * rhs; }
		void     operator/=(DynamicPeriodic rhs) const { *this = *this / rhs; }

		explicit operator T() const
		{ return mValue; }

	private:
		T mMax;
		T mValue;
	};
}

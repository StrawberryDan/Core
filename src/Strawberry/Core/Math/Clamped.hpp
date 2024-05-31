#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include <algorithm>
#include <concepts>


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
                , mValue(std::clamp<T>(value, mMin, mMax)) {}


            operator T() const
            {
                return mValue;
            }


            T operator*() const
            {
                return mValue;
            }


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


            Clamped& operator++()
            {
                *this += 1;
                return *this;
            }


            Clamped& operator--()
            {
                *this -= 1;
                return *this;
            }


            Clamped operator++(int)
            {
                Clamped copy = *this;
                *this += 1;
                return copy;
            }


            Clamped operator--(int)
            {
                Clamped copy = *this;
                *this -= 1;
                return copy;
            }

        private:
            T mMin;
            T mMax;
            T mValue;
    };


    template<typename T, T MIN, T MAX> requires std::integral<T> || std::floating_point<T>
    class StaticClamped
    {
        public:
            StaticClamped(T value = T{})
                : mValue(std::clamp<T>(value, MIN, MAX)) {}


            operator T() const
            {
                return mValue;
            }


            T operator*() const
            {
                return mValue;
            }


            auto operator<=>(const StaticClamped&) const = default;


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


            StaticClamped operator+(const T& rhs) const
            {
                return StaticClamped(mValue + rhs);
            }


            StaticClamped operator-(const T& rhs) const
            {
                return StaticClamped(mValue - rhs);
            }


            StaticClamped operator*(const T& rhs) const
            {
                return StaticClamped(mValue * rhs);
            }


            StaticClamped operator/(const T& rhs) const
            {
                return StaticClamped(mValue / rhs);
            }


            StaticClamped& operator+=(const T& rhs)
            {
                mValue += rhs;
                mValue = std::clamp<T>(mValue, MIN, MAX);
                return *this;
            }


            StaticClamped& operator-=(const T& rhs)
            {
                mValue -= rhs;
                mValue = std::clamp<T>(mValue, MIN, MAX);
                return *this;
            }


            StaticClamped& operator*=(const T& rhs)
            {
                mValue *= rhs;
                mValue = std::clamp<T>(mValue, MIN, MAX);
                return *this;
            }


            StaticClamped& operator/=(const T& rhs)
            {
                mValue /= rhs;
                mValue = std::clamp<T>(mValue, MIN, MAX);
                return *this;
            }


            StaticClamped operator+(const StaticClamped& rhs) const
            {
                return StaticClamped(mValue + rhs.mValue);
            }


            StaticClamped operator-(const StaticClamped& rhs) const
            {
                return StaticClamped(mValue - rhs.mValue);
            }


            StaticClamped operator*(const StaticClamped& rhs) const
            {
                return StaticClamped(mValue * rhs.mValue);
            }


            StaticClamped operator/(const StaticClamped& rhs) const
            {
                return StaticClamped(mValue / rhs.mValue);
            }


            StaticClamped& operator+=(const StaticClamped& rhs)
            {
                mValue += rhs;
                mValue = std::clamp<T>(mValue, MIN, MAX);
                return *this;
            }


            StaticClamped& operator-=(const StaticClamped& rhs)
            {
                mValue -= rhs;
                mValue = std::clamp<T>(mValue, MIN, MAX);
                return *this;
            }


            StaticClamped& operator*=(const StaticClamped& rhs)
            {
                mValue *= rhs;
                mValue = std::clamp<T>(mValue, MIN, MAX);
                return *this;
            }


            StaticClamped& operator/=(const StaticClamped& rhs)
            {
                mValue /= rhs;
                mValue = std::clamp<T>(mValue, MIN, MAX);
                return *this;
            }


            StaticClamped& operator++()
            {
                *this += 1;
                return *this;
            }


            StaticClamped& operator--()
            {
                *this -= 1;
                return *this;
            }


            StaticClamped operator++(int)
            {
                StaticClamped copy = *this;
                *this += 1;
                return copy;
            }


            StaticClamped operator--(int)
            {
                StaticClamped copy = *this;
                *this -= 1;
                return copy;
            }

        private:
            T mValue;
    };
}

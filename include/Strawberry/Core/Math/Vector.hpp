#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Units.hpp"
// Standard Library
#include <concepts>

//======================================================================================================================
//  Class Definition
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Math
{
	template <typename T, size_t D>
		requires (std::signed_integral<T> || std::floating_point<T>)
	class Vector
	{
	public:
		/// Default constructor makes all zeroes
		constexpr Vector() noexcept
			: mValue{0}
		{}

		/// Value constructor. Initialises specified values. Unspecified values are zeroes.
		template <typename... Args>
			requires (sizeof...(Args) <= D && (std::same_as<T, Args> && ...))
		constexpr explicit Vector(Args... args) noexcept
			: mValue{args...}
		{}

		/// Construction from another vector. If the other vector is smaller, excess values are zeroes.
		template <typename T2, size_t D2>
			requires (D2 <= D)
		constexpr explicit Vector(const Vector<T2, D2>& other) noexcept
		{
			size_t i = 0;
			for (; i < D2; i++) mValue[i] = T(other[i]);
			for (; i < D; i++) mValue[i] = T(0);
		}

		/// Mutable accessor
		constexpr T& operator[](size_t i) noexcept { return mValue[i]; }

		/// Immutable accessor
		constexpr const T& operator[](size_t i) const noexcept { return mValue[i]; }

		/// Define Vector Equality
		friend constexpr bool operator==(const Vector& a, const Vector& b)
		{
			for (size_t i = 0; i < D; i++)
			{
				if (a[i] != b[i]) return false;
			}

			return true;
		}

		/// Define Vector Inequality
		friend constexpr bool operator!=(const Vector& a, const Vector& b)
		{
			for (size_t i = 0; i < D; i++)
			{
				if (a[i] == b[i]) return false;
			}

			return true;
		}

		/// Define addition
		constexpr friend Vector operator+(const Vector& a, const Vector& b) noexcept
		{
			Vector result;
			for (size_t i = 0; i < D; i++) result[i] = a[i] + b[i];
			return result;
		}

		/// Define subtraction
		constexpr friend Vector operator-(const Vector& a, const Vector& b) noexcept
		{
			Vector result;
			for (size_t i = 0; i < D; i++) result[i] = a[i] - b[i];
			return result;
		}

		/// Define multiplication
		constexpr friend Vector operator*(const Vector& a, T b) noexcept
		{
			Vector result;
			for (size_t i = 0; i < D; i++) result[i] = a[i] * b;
			return result;
		}

		/// Define commutative multiplication
		constexpr friend Vector operator*(T a, const Vector& b) noexcept { return b * a; }

		/// Define division by scalars
		constexpr friend Vector operator/(const Vector& a, T b) noexcept { return a * (T(1) / b); }

		/// Define Magnitude
		double Magnitude() const noexcept { return std::sqrt(SquareMagnitude()); }

		/// Define Square Magnitude
		T SquareMagnitude() const noexcept
		{
			T result(0);
			for (size_t i = 0; i < D; i++) result += mValue[i] * mValue[i];
			return result;
		}

		/// Define vector normalisation
		Vector Normalised() const noexcept
			requires (std::floating_point<T>)
		{
			return *this / Magnitude();
		}

		/// Define Dot Product
		constexpr T Dot(const Vector& b) const noexcept
		{
			T result(0);
			for (size_t i = 0; i < D; i++) result += mValue[i] * b[i];
			return result;
		}

		/// Define Cross Product
		constexpr Vector Cross(const Vector& b) const noexcept
			requires (D == 3)
		{
			return Vector(mValue[1] * b[2] - mValue[2] * b[1], mValue[2] * b[0] - mValue[0] * b[2], mValue[1] * b[2] - mValue[2] * b[1]);
		}

		/// Calculate the angle between this and another vector in radians.
		Radians AngleBetween(const Vector& b) const noexcept { return std::acos(Dot(b) / std::sqrt(SquareMagnitude() * b.SquareMagnitude())); }

		/// Gets the vector after it is projected onto a plane oriented by the given normal.
		Vector ProjectOntoPlane(const Vector& normal) const noexcept
			requires (std::floating_point<T>)
		{
			return *this - normal.Normalised() * (this->Dot(normal));
		}


	private:
		T mValue[D];
	};

	template <typename T, typename... Args>
	Vector(T, Args...) -> Vector<T, 1 + sizeof...(Args)>;
} // namespace Strawberry::Core::Math

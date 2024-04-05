#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Units.hpp"
// Standard Library
#include <concepts>
#include <cmath>
#include <memory>
#include <tuple>

//======================================================================================================================
//  Class Definition
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Math
{
	template<typename T, size_t D>
	requires (std::integral<T> || std::floating_point<T>)
	class Vector
	{
	public:
		/// Default constructor makes all zeroes
		constexpr Vector() noexcept
			: mValue{0} {}


		/// Value constructor. Initialises specified values. Unspecified values are zeroes.
		template<typename... Args>
		requires (sizeof...(Args) == D && (std::convertible_to<Args, T> && ...))
		constexpr explicit(sizeof...(Args) == 1) Vector(Args... args) noexcept
			: mValue{static_cast<T>(args)...} {}


		/// Construction from another vector. If the other vector is smaller, excess values are zeroes.
		template<typename T2, size_t D2>
		constexpr explicit Vector(const Vector<T2, D2>& other) noexcept
		{
			for (int i = 0; i < D; i++)
			{
				(*this)[i] = (i < D2) ? other[i] : T(0);
			}
		}


		/// Copy Conversion to a different sized vector
		template<size_t D2>
		constexpr Vector<T, D2> AsSize() const noexcept
		{
			return Vector<T, D2>(*this);
		}



		/// Static casts between vector types
		template <typename T2>
		constexpr Vector<T2, D> AsType() const noexcept
		{
			Vector<T2, D> result;
			for (int i = 0; i < D; i++)
				result[i] = static_cast<T2>((*this)[i]);
			return result;
		}


		/// Offsets the vector by the given args. Shorthand for adding a constant vector.
		template<typename... Args> requires (sizeof...(Args) == D && (std::convertible_to<T, Args> && ...))
		constexpr Vector<T, D> Offset(Args... args) const noexcept
		{
			Vector<T, D> offset(static_cast<Args>(args)...);
			return *this + offset;
		}


		template <typename... Args> requires (std::convertible_to<Args, T> && ...)
		constexpr Vector<T, D + sizeof...(Args)> WithAdditionalValues(Args... args) const
		{
			Vector<T, sizeof...(Args)> argsAsVector(args...);
			Vector<T, D + sizeof...(Args)> result(*this);
			for (int i = 0; i < sizeof...(Args); i++)
				result[D + i] = argsAsVector[i];
			return result;
		};


		/// Mutable accessor
		constexpr T& operator[](size_t i) noexcept { return mValue[i]; }


		/// Immutable accessor
		constexpr const T& operator[](size_t i) const noexcept { return mValue[i]; }


		/// Apply function to all members
		template <typename F>
		constexpr Vector Map(F function) const noexcept
		{
			Vector result;
			for (int i = 0; i < D; i++) result[i] = function((*this)[i]);
			return result;
		}


		/// get implementation for structured binding
		template<size_t I>
		constexpr T& get()
		{
			return mValue[I];
		}


		template<size_t I>
		constexpr const T& get() const
		{
			return mValue[I];
		}


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


		/// Define scalar multiplication
		constexpr friend Vector operator*(const Vector& a, T b) noexcept
		{
			Vector result;
			for (size_t i = 0; i < D; i++) result[i] = a[i] * b;
			return result;
		}


		/// Define commutative scalar multiplication
		constexpr friend Vector operator*(T a, const Vector& b) noexcept { return b * a; }


		/// Define Component-wise multiplication
		constexpr friend Vector operator*(const Vector& a, const Vector& b)
		{
			Vector result;
			for (int i = 0; i < D; i++) result[i] = a[i] * b[i];
			return result;
		}


		/// Define division by scalars
		constexpr friend Vector operator/(const Vector& a, T b) noexcept { return a * (T(1) / b); }


		/// Define Component-wise division
		constexpr friend Vector operator/(const Vector& a, const Vector& b)
		{
			Vector result;
			for (int i = 0; i < D; i++) result[i] = a[i] / b[i];
			return result;
		}


		/// Define addition assignment
		Vector& operator+=(const Vector& rhs) noexcept
		{
			*this = *this + rhs;
			return *this;
		}


		/// Define subtraction assignment
		Vector& operator-=(const Vector& rhs) noexcept
		{
			*this = *this - rhs;
			return *this;
		}


		/// Define multiplication assignment
		Vector& operator*=(const Vector& rhs) noexcept
		{
			*this = *this * rhs;
			return *this;
		}


		/// Define scalar division assignment
		Vector& operator/=(const Vector& rhs) noexcept
		{
			*this = *this / rhs;
			return *this;
		}


		/// Define Magnitude
		[[nodiscard]] double Magnitude() const noexcept { return std::sqrt(SquareMagnitude()); }


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
			auto [x1, y1, z1] = *this;
			auto [x2, y2, z2] = b;
			return Vector(y1 * z2 - z1 * y2, z1 * x2 - x1 * z2, x1 * y2 - y1 * x2);
		}


		/// Calculate the angle between this and another vector in radians.
		Radians AngleBetween(const Vector& b) const noexcept
		{
			return std::acos(Dot(b) / std::sqrt(SquareMagnitude() * b.SquareMagnitude()));
		}


		/// Gets the vector after it is projected onto a plane oriented by the given normal.
		Vector ProjectOntoPlane(const Vector& normal) const noexcept
		requires (std::floating_point<T>)
		{
			return *this - normal.Normalised() * (this->Dot(normal));
		}


	private:
		T mValue[D];
	};

	//======================================================================================================================
	//  Deduction Guide
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T, typename... Args>
	Vector(T, Args...) -> Vector<T, 1 + sizeof...(Args)>;

	//======================================================================================================================
	//  Type Aliases for Common Use
	//----------------------------------------------------------------------------------------------------------------------
	using Vec2 = Vector<double, 2>;
	using Vec3 = Vector<double, 3>;
	using Vec4 = Vector<double, 4>;
	using Vec2f = Vector<float, 2>;
	using Vec3f = Vector<float, 3>;
	using Vec4f = Vector<float, 4>;
	using Vec2i = Vector<int, 2>;
	using Vec3i = Vector<int, 3>;
	using Vec4i = Vector<int, 4>;
	using Vec2u = Vector<unsigned int, 2>;
	using Vec3u = Vector<unsigned int, 3>;
	using Vec4u = Vector<unsigned int, 4>;
} // namespace Strawberry::Core::Math


// Standard Library Helper Structs
namespace std
{
	// For Destructuring
	template<size_t I, typename T, size_t D>
	struct tuple_element<I, Strawberry::Core::Math::Vector<T, D>>
	{
		using type = T;
	};

	// For Destructuring
	template<typename T, size_t D>
	struct tuple_size<Strawberry::Core::Math::Vector<T, D>>
		: public std::integral_constant<size_t, D>
	{};

	// For Hashing
	template<typename T, size_t D>
	struct hash<Strawberry::Core::Math::Vector<T, D>>
	{
		std::size_t operator()(const Strawberry::Core::Math::Vector<T, D>& value) const noexcept
		{
			std::size_t hash = 0;
			for (unsigned int i = 0; i < D; i++) hash = hash xor std::hash<T>()(value[i]);
			return hash;
		}
	};
} // namespace std

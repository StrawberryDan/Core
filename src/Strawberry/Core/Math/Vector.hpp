#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Units.hpp"
#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/Markers.hpp"
// Standard Library
#include <concepts>
#include <cmath>
#include <functional>
#include <ranges>
#include <tuple>
#include <sstream>


//======================================================================================================================
//  Class Definition
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Math
{
	template<typename T, size_t D> requires (std::integral<T> || std::floating_point<T>)
	class Vector
	{
	public:
		/// Default constructor makes all zeroes
		constexpr Vector() noexcept
			: mValue{0} {}


		/// Value constructor. Initialises specified values. Unspecified values are zeroes.
		template<typename... Args> requires (sizeof...(Args) == D && (std::convertible_to<Args, T> && ...))
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


		/// Returns this vector without the first N elements.
		template <size_t N> requires (D - N >= 1)
		constexpr Vector<T, D - N> Skip() const noexcept
		{
			Vector<T, D - N> res;
			for (int i = N; i < D; i++)
			{
				res[i - N] = (*this)[i];
			}
			return res;
		}


		/// Copy Conversion to a different sized vector
		template<size_t D2>
		constexpr Vector<T, D2> AsSize() const noexcept
		{
			return Vector<T, D2>(*this);
		}


		/// Static casts between vector types
		template<typename T2>
		constexpr Vector<T2, D> AsType() const noexcept
		{
			Vector<T2, D> result;
			for (int i    = 0; i < D; i++)
				result[i] = static_cast<T2>((*this)[i]);
			return result;
		}


		std::string ToString() const noexcept
		{
			std::stringstream ss;
			ss << "(" << mValue[0];
			for (int i = 1; i < D; i++)
			{
				ss << ", " << mValue[i];
			}
			ss << ")";
			return ss.str();
		}


		/// Offsets the vector by the given args. Shorthand for adding a constant vector.
		template<typename... Args> requires (sizeof...(Args) == D && (std::convertible_to<T, Args> && ...))
		constexpr Vector<T, D> Offset(Args... args) const noexcept
		{
			Vector<T, D> offset(static_cast<Args>(args)...);
			return *this + offset;
		}


		template <size_t D2>
		constexpr Vector<T, D + D2> AppendedWith(const Vector<T, D2>& args) noexcept
		{
			Vector<T, D + D2> res;
			for (int i = 0; i < D + D2; i++)
			{
				if (i < D)
				{
					res[i] = (*this)[i];
				}
				else
				{
					res[i] = args[i - D];
				}
			}
			return res;
		}


		/// Appends additional values to the end of the vector
		template<typename... Args> requires (std::convertible_to<Args, T> && ...)
		constexpr Vector<T, D + sizeof...(Args)> AppendedWith(Args... args) const
		{
			Vector<T, sizeof...(Args)>     argsAsVector(args...);
			Vector<T, D + sizeof...(Args)> result(*this);
			for (int i        = 0; i < sizeof...(Args); i++)
				result[D + i] = argsAsVector[i];
			return result;
		};


		/// Flattens the given position into a 1D index lexicographically.
		/// Increasing x[D -1] by one increases Flatten(x) by 1.
		constexpr T Flatten(Vector x) const noexcept
		{
			if constexpr (D == 1)
			{
				return x[0];
			}
			else
			{
				T flattened = x[0] * Skip<1>().Fold(std::multiplies()) + Skip<1>().Flatten(x.Skip<1>());
				return flattened;
			}
		}


		/// Returns the reverse lexicographical flattening of x within this rectangle.
		/// Increasing x[0] by one increases FlattenR(x) by 1.
		constexpr T FlattenR(Vector x) const noexcept
		{
			return Reversed().Flatten(x.Reversed());
		}



		/// Unflattens an index into a rectangular space lexicographically.
		/// Incrementing i by 1 increments Unflatten(i) from the right.
		constexpr Vector Unflatten(T i) const noexcept
		{
			if constexpr (D == 1)
			{
				return Vector(i);
			}
			else
			{
				T interval = Skip<1>().Fold(std::multiplies());
				Vector<T, D - 1> subEmbedding = Skip<1>().Unflatten(i % interval);

				Vector unflattened = Vector<T, 1>(i / interval).AppendedWith(subEmbedding);
				return unflattened;
			}
		}


		/// Unflattens an index into a rectangular space lexicographically.
		/// Incrementing i by 1 increments UnflattenR(i) from the left.
		constexpr Vector UnflattenR(T i) const noexcept
		{
			return Reversed().Unflatten(i).Reversed();
		}


		static std::vector<Vector> Rectangle(Vector size) requires (std::unsigned_integral<T>)
		{
			auto addDimRange = [](std::vector<Vector<T, D - 1>>&& acc, std::vector<T>&& dimension ) -> std::vector<Vector<T, D>>
			{
				std::vector<Vector> results;

				for (auto&& v : acc)
				{
					for (auto&& d : dimension)
					{
						results.emplace_back(v.AppendedWith(d));
					}
				}

				return results;
			};

			std::vector<T> dimension;
			for (int i = 0; i < size[D - 1]; i++)
			{
				dimension.emplace_back(i);
			}

			if constexpr (D == 1)
			{
				return dimension | std::views::transform([](auto&& x) -> Vector { return Vector(x); }) | std::ranges::to<std::vector>();
			}
			else
			{
				Vector<T, D - 1> subVector(size);
				return addDimRange(subVector.Rectangle(), std::move(dimension));
			}
		}


		std::vector<Vector> Rectangle() const requires(std::unsigned_integral<T>)
		{
			return Rectangle(*this);
		}


		/// Mutable accessor
		constexpr T& operator[](size_t i) noexcept
		{
			return mValue[i];
		}


		/// Immutable accessor
		constexpr const T& operator[](size_t i) const noexcept
		{
			return mValue[i];
		}


		/// Fold the vectors with the given operation
		template <typename F>
		constexpr auto Fold(F&& function) const noexcept
		{
			if constexpr (D == 1)
			{
				return mValue[0];
			}
			else if constexpr (D >= 2)
			{
				auto acc = mValue[0];
				for (int i = 1; i < D; i++)
				{
					acc = std::invoke(function, acc, mValue[i]);
				}
				return acc;
			}
		}


		/// Returns this vector in reverse
		constexpr Vector Reversed() const noexcept
		{
			Vector res;
			for (int i = 0; i < D; i++)
			{
				res[i] = (*this)[D - i - 1];
			}
			return res;
		}


		template <typename... Args>
		constexpr Vector<T, sizeof...(Args)> Swizzle(Args... args)
		{
			std::array _args{args...};
			Vector<T, sizeof...(Args)> result;
			for (int i = 0; i < sizeof...(Args); i++)
				result[i] = (*this)[_args[i]];
			return result;
		}


		/// Apply function to all members
		template<typename F>
		constexpr Vector<std::invoke_result_t<F, T>, D> Map(F&& function) const noexcept
		{
			Vector<std::invoke_result_t<F, T>, D> result;
			for (int i = 0; i < D; i++) result[i] = std::invoke(std::forward<F>(function), (*this)[i]);
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

		// Default comparison operators
		auto operator<=>(const Vector& other) const = default;


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
		constexpr friend Vector operator*(T a, const Vector& b) noexcept
		{
			return b * a;
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


		[[nodiscard]] bool IsZero() const noexcept
		{
			bool allZero = true;
			for (int i = 0; i < D; i++)
			{
				if (std::fpclassify(mValue[i]) != FP_ZERO)
				{
					allZero = false;
				}
			}
			return allZero;
		}


		/// Define Magnitude
		[[nodiscard]] double Magnitude() const noexcept
		{
			return std::sqrt(SquareMagnitude());
		}


		/// Define Square Magnitude
		T SquareMagnitude() const noexcept
		{
			T result(0);
			for (size_t i = 0; i < D; i++) result += mValue[i] * mValue[i];
			return result;
		}


		/// Define vector normalisation
		Vector Normalised() const noexcept requires (std::floating_point<T>)
		{
			Vector result = *this * (1.0 / Magnitude());
			for (int i = 0; i < D; i++)
			{
				Core::Assert(std::isfinite((*this)[i]));
			}
			return result;
		}


		Vector NormalisedOrZero() const noexcept requires (std::floating_point<T>)
		{
			if (IsZero())
			{
				return Vector();
			}
			else
			{
				return Normalised();
			}
		}


		Vector WithLength(T length) const noexcept requires (std::floating_point<T>)
		{
			auto magnitude = Magnitude();
			return this->Map([=] (auto&& x) -> T { return length / magnitude * x; });
		}


		Vector WithLengthOrZero(T length) const noexcept requires (std::floating_point<T>)
		{
			if (IsZero())
			{
				return Vector();
			}
			else
			{
				return WithLength(length);
			}
		}


		/// Define Dot Product
		constexpr T Dot(const Vector& b) const noexcept
		{
			T result(0);
			for (size_t i = 0; i < D; i++) result += mValue[i] * b[i];
			return result;
		}


		/// Define Cross Product
		constexpr Vector Cross(const Vector& b) const noexcept requires (D == 3)
		{
			auto [x1, y1, z1] = *this;
			auto [x2, y2, z2] = b;
			return Vector(y1 * z2 - z1 * y2, z1 * x2 - x1 * z2, x1 * y2 - y1 * x2);
		}


		/// De-homogenize a homogenous coordinate
		constexpr Vector<T, D - 1> Dehomogenize() const noexcept requires (D > 1)
		{
			const float w = (*this)[D - 1];

			// If w is zero then we return without dividing
			if (std::fpclassify(w) == FP_ZERO)
			{
				return AsSize<D - 1>();
			}

			return this->Map([=] (auto&& x) { return x / w; }).template AsSize<D - 1>();
		}


		/// Returns the counterclockwise perpendicular vector to this one.
		constexpr Vector<T, D> Perpendicular() const noexcept requires (D == 2)
		{
			return { -mValue[1], mValue[0] };
		}


		/// Calculate the angle between this and another vector in radians.
		Radians AngleBetween(const Vector& b) const noexcept
		{
			return std::acos(Dot(b) / std::sqrt(SquareMagnitude() * b.SquareMagnitude()));
		}


		/// Gets the vector after it is projected onto a plane oriented by the given normal.
		Vector ProjectOntoPlane(const Vector& normal) const noexcept requires (std::floating_point<T>)
		{
			return *this - normal.Normalised() * (this->Dot(normal));
		}

		[[nodiscard]] unsigned int Embed() const noexcept requires (std::same_as<T, int> && D == 2)
		{
			auto L_high = [](unsigned int x) { return (2 * x + 1) * (2 * x + 1); };
			auto L_low  = [](unsigned int x) { if (x == 0) return 0u; return (2 * x - 1) * (2 * x - 1); };

			auto S = [](int x, int y) { return std::max<unsigned int>({0u, static_cast<unsigned int>(std::abs(x)), static_cast<unsigned int>(std::abs(y))}); };
			auto W = [L_high, L_low](unsigned int x) { return (L_high(x) - L_low(x)) / 4; };
			auto Q = [](int x, int y)
			{
				if (x >= 0 && y >= 0) return 0;
				if (x <  0 && y >= 0) return 1;
				if (x <  0) return 2;
				return 3;
			};

			auto R = [S](unsigned int x, unsigned int y)
			{
				if (x >= y) return y;
				return 2u * S(x, y) - x;
			};

			auto [x, y] = mValue;

			unsigned int embeding = L_low(S(x, y)) + Q(x, y) * W(S(x, y));
			switch (Q(x, y))
			{
			case 0: return embeding + R(x, y);

			case 1: return embeding + R(y, -x);

			case 2: return embeding + R(-x, -y);

			case 3: return embeding + R(-y, x);

			default:
				Unreachable();
			}

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
	using Vec2  = Vector<double, 2>;
	using Vec3  = Vector<double, 3>;
	using Vec4  = Vector<double, 4>;
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


namespace fmt
{
	template <typename T, size_t D>
	struct formatter<Strawberry::Core::Math::Vector<T, D>>
		: formatter<std::string>
	{
		using formatter<std::string>::parse;

		auto format(const Strawberry::Core::Math::Vector<T, D>& v, format_context& ctx) const -> format_context::iterator
		{
			auto out = ctx.out();
			if constexpr (D == 1)
			{
				out = fmt::format_to(out, "[{}]", v[0]);
			}
			else
			{
				std::string str = fmt::format("[{}", v[0]);
				for (int i = 1; i < D; i++)
				{
					str = fmt::format("{}, {}", str, v[i]);
				}
				str = fmt::format("{}]", str);
				out = fmt::format_to(out, "{}", str);
			}

			return out;
		}
	};
}



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
			: public std::integral_constant<size_t, D> {};


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



#pragma once


#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/Optional.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned int D>
	class Line
	{
	public:
		Line() = default;
		Line(const Vector<T, D>& a, const Vector<T, D>& b)
			: mPoints{a, b} {}


		Vector<T, D>& A() { return mPoints[0]; }
		const Vector<T, D>& A() const { return mPoints[0]; }
		Vector<T, D>& B() { return mPoints[1]; }
		const Vector<T, D>& B() const { return mPoints[1]; }

		Vector<T, D> Direction() const noexcept { return B() - A(); }


		Optional<Vector<T, D>> Intersection(const Line& other) const noexcept requires (D == 2)
		{
			// Solve equation system using Cramer's rule.
			auto& p1 = A();
			auto& p2 = other.A();
			auto   c = p2 - p1;
			auto  v1 = Direction();
			auto  v2 = other.Direction();

			double determinant = v1[0] * v2[1] - v2[0] * v1[1];
			if (determinant == 0.0)
			{
				return NullOpt;
			}

			auto t1Num = c[0] * v2[1] - v2[0] * c[1];
			auto t1 = t1Num / determinant;
			return p1 + t1 * v1;
		}

		auto operator<=>(const Line& line) const = default;


		std::string ToString() const noexcept
		{
			return fmt::format("Line({} --> {})", A().ToString(), B().ToString());
		}


	private:
		std::array<Vector<T, D>, 2> mPoints;
	};}


namespace fmt
{
	template <typename T, size_t D>
	struct formatter<Strawberry::Core::Math::Line<T, D>> : formatter<std::string>
	{
		using formatter<std::string>::parse;

		auto format(const Strawberry::Core::Math::Line<T, D>& l, format_context& ctx) const -> format_context::iterator
		{
			return fmt::format_to(ctx.out(), "Line({} -> {})", l.A(), l.B());
		}
	};

	template <typename T, unsigned D>
	struct formatter<Strawberry::Core::Math::LineSegment<T, D>> : formatter<std::string>
	{
		using formatter<std::string>::parse;

		auto format(const Strawberry::Core::Math::LineSegment<T, D>& l, format_context& ctx) const -> format_context::iterator
		{
			return fmt::format_to(ctx.out(), "LineSegment({} -> {})", l.A(), l.B());
		}
	};
}

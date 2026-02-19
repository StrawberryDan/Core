#pragma once



#include "Strawberry/Core/IO/Logging.hpp"
#include "Strawberry/Core/Math/Geometry/Simplex.hpp"
#include "Strawberry/Core/Math/Geometry/Line.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include <ranges>


namespace Strawberry::Core::Math
{
	template <typename T, unsigned D>
	class Sphere
	{
	public:
		template <unsigned int O>
		static Core::Optional<Sphere> Circumcsphere(const Simplex<T, D, O>& s)
		{
			Sphere sphere;

			auto lines = s.GetLineSegments()
				| std::views::transform([] (LineSegment<T, D> l)
				{
					return Line<T, D>(l.Midpoint(), l.Midpoint() + l.Direction().Perpendicular());
				})
				| std::ranges::to<std::vector>();


			auto intersection = lines[0].Intersection(lines[1]);
			if (intersection)
			{
				sphere.Center() = intersection.Value();
				sphere.Radius() = (s.Point(0) - sphere.Center()).Magnitude();
				return sphere;
			}
			else
			{
				return NullOpt;
			}
		}


		Sphere() = default;
		Sphere(const Vector<T, D>& center, T radius)
			: mCenter(center), mRadius(radius) {}


		      Vector<T, D>& Center()       { return mCenter; }
		const Vector<T, D>& Center() const { return mCenter; }

		double& Radius()       { return mRadius; }
		double  Radius() const { return mRadius; }


		bool Contains(const Vector<T, D>& point) const
		{
			return (point - Center()).SquareMagnitude() < mRadius * mRadius;
		}


	private:
		Vector<T, D> mCenter;
		T            mRadius;
	};
}


namespace fmt
{
	template <typename T, unsigned D>
	struct formatter<Strawberry::Core::Math::Sphere<T, D>> : formatter<std::string>
	{
		using formatter<std::string>::parse;

		auto format(const Strawberry::Core::Math::Sphere<T, D>& s, format_context& ctx) -> format_context::iterator
		{
			return fmt::format_to(ctx.out(), "Sphere({} -- {})", s.Center(), s.Radius());
		}
	};
}

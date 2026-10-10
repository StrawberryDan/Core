#pragma once


#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Math/Geometry/LineSegment.hpp"
#include "Strawberry/Core/Math/Geometry/Sphere.hpp"
#include "Strawberry/Core/Math/Geometry/Shape.hpp"
#include <array>
#include <set>


namespace Strawberry::Core::Math::Geometry
{
    template <typename T>
    class ConvexPolygon;


    template <typename T, unsigned Dimension, unsigned Order> requires (Order <= Dimension + 1)
    class Simplex
        : public Shape
    {
    public:
        Simplex() = default;


        template <std::ranges::sized_range Range>
        requires (std::convertible_to<std::ranges::range_value_t<Range>, Vector<T, Dimension>>)
        Simplex(Range&& points) requires (std::ranges::size(points) == Order)
            : mPoints{points}
        {
            if constexpr (Dimension == 2 && Order == 3)
            {
                MakeCounterClockwise();
            }
        }


        template <typename... Args>
        Simplex(Args&&... args) requires ( sizeof...(Args) == Order && ( std::same_as<std::decay_t<Args>, Vector<T, Dimension>> && ... ) )
            : Simplex(std::array<Vector<T, Dimension>, sizeof...(Args)>{std::forward<Args>(args)...})
        {}


        Vector<T, Dimension>& Point(unsigned index) { return mPoints[index]; }
        const Vector<T, Dimension>& Point(unsigned index) const { return mPoints[index]; }
        decltype(auto) Points() const { return mPoints | std::views::all; }

        bool Contains(const Vector<T, Dimension>& point) const requires (Dimension == 2 && Order == 3)
        {
            if ((Point(1) - Point(0)).Perpendicular().Dot(point - Point(0)) < 0) [[likely]] return false;
            if ((Point(2) - Point(1)).Perpendicular().Dot(point - Point(1)) < 0) [[likely]] return false;
            if ((Point(0) - Point(2)).Perpendicular().Dot(point - Point(2)) < 0) [[likely]] return false;
            return true;
        }


        /// Robust test for whether 'point' lies inside this (counter-clockwise
        /// ordered, guaranteed by the constructor) triangle's circumcircle,
        /// using the standard incircle determinant. Unlike going via
        /// GetCircumsphere(), this never needs to construct an explicit
        /// circumcenter: that requires intersecting two perpendicular
        /// bisectors, which is numerically fragile for thin/obtuse triangles
        /// (the true circumcenter of a near-degenerate triangle is unbounded),
        /// and has no principled answer at all for an exactly degenerate one.
        /// This determinant needs no such fallback: for an exactly degenerate
        /// (collinear, zero-area) triangle it evaluates to exactly zero, so it
        /// correctly reports "not contained" rather than being undefined.
        bool CircumcircleContains(const Vector<T, Dimension>& point) const noexcept requires (Dimension == 2 && Order == 3)
        {
            double ax = Point(0)[0] - point[0], ay = Point(0)[1] - point[1];
            double bx = Point(1)[0] - point[0], by = Point(1)[1] - point[1];
            double cx = Point(2)[0] - point[0], cy = Point(2)[1] - point[1];

            double det =
                (ax * ax + ay * ay) * (bx * cy - by * cx) -
                (bx * bx + by * by) * (ax * cy - ay * cx) +
                (cx * cx + cy * cy) * (ax * by - ay * bx);

            return det > 0.0;
        }


        /// Calculates the mean of the vertices of this Simplex.
        Vector<T, Dimension> GetMean() const noexcept
        {
            Vector<T, Dimension> mean;

            for (unsigned int i = 0; i < Order; i++)
            {
                mean = mean + Point(i);
            }

            return (1.0 / Order) * mean;
        }


        [[nodiscard]] ConvexPolygon<T> AsPolygon() const requires (Dimension == 2)
        {
            return ConvexPolygon<T>::From(mPoints);
        }


        [[nodiscard]] double Area() const noexcept requires (Order == 3 && Dimension == 2)
        {
            return 0.5 * std::abs(
                Point(0)[0] * (Point(1)[1] - Point(2)[1]) +
                Point(1)[0] * (Point(2)[1] - Point(0)[1]) +
                Point(2)[0] * (Point(0)[1] - Point(1)[1]));
        }


        bool IsCounterClockwise() const requires (Dimension == 2 && Order == 3)
        {
            auto a = Point(1) - Point(0);
            auto b = Point(2) - Point(1);

            return b.Dot(a.Perpendicular()) >= 0.0;
        }


        void MakeCounterClockwise() requires (Dimension == 2 && Order == 3)
        {
            if (!IsCounterClockwise())
            {
                std::swap(Point(1), Point(2));
            }

            Core::Assert(IsCounterClockwise());
        }


        std::set<LineSegment<T, Dimension>> GetLineSegments() const
        {
            if constexpr (Dimension == 2 && Order == 3)
            {
                return {
                    LineSegment<T, Dimension>(Point(0), Point(1)),
                    LineSegment<T, Dimension>(Point(1), Point(2)),
                    LineSegment<T, Dimension>(Point(2), Point(0))
                };
            }
            else
            {
                std::set<LineSegment<T, Dimension>> lines;
                for (int i = 0; i < Order; i++)
                {
                    for (int j = i + 1; j < Order; j++)
                    {
                        lines.emplace(Point(i), Point(j));
                    }
                }

                Core::AssertEQ(lines.size(), (Order * (Order - 1)) / 2);
                return lines;
            }
        }


        Optional<Sphere<T, Dimension>> GetCircumsphere() const requires (Dimension == 2 && Order == 3)
        {
            auto perps = GetLineSegments() | std::views::take(2) | std::views::transform([] (LineSegment<T, Dimension> l)
            {
                return Line<T, Dimension>(l.Midpoint(), l.Midpoint() + l.Direction().Perpendicular());
            }) | std::ranges::to<std::vector>();

            auto center = perps[0].Intersection(perps[1]);

            if (center)
            {
                auto radius = (Point(0) - center->position).Magnitude();
                return Sphere<T, Dimension>(center->position, radius);
            }

            return NullOpt;
        }


    private:
        std::array<Vector<T, Dimension>, Order> mPoints;
    };


    template <typename T, unsigned int D, unsigned int O>
    struct Translation<Simplex<T, D, O>>
    {
        Translation(auto&& x)
            : parameter(std::forward<decltype(x)>(x))
        {}


        Simplex<T, D, O> operator()(const Simplex<T, D, O>& x) const
        {
            const auto transformPoint = [&](auto&& x) { return x + parameter; };
            return Simplex<T, D, O>(x.Points() | std::views::transform(transformPoint));
        }


        Vector<T, D> parameter;
    };


    template <typename T, unsigned Dimension>
    using Triangle = Simplex<T, Dimension, 3>;
}


namespace fmt
{
    template <typename T, unsigned int D, unsigned int O>
    struct formatter<Strawberry::Core::Math::Geometry::Simplex<T, D, O>>
        : public formatter<std::string>
    {
        using formatter<std::string>::parse;


        auto format(const Strawberry::Core::Math::Geometry::Simplex<T, D, O>& s, format_context& ctx) const -> format_context::iterator
        {
            auto out = ctx.out();
            out = fmt::format_to(out, "Simplex(");

            for (int i = 0; i < O; i++)
            {
                out = fmt::format_to(out, "{}", s.Point(i));
                if (i < O - 1) out = fmt::format_to(out, " --- ");
            }

            out = fmt::format_to(out, ")");

            return out;
        }
    };
}

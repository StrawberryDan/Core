#pragma once


#include "Ray.hpp"
#include "Shape.hpp"
#include "Simplex.hpp"
#include <concepts>
#include <ranges>
#include <vector>


namespace Strawberry::Core::Math::Geometry
{
    template <typename T, unsigned int D>
    class PointSet;


    template <typename T>
    class ConvexPolygon
        : public Shape
    {
    public:
        ConvexPolygon() = default;


        template <std::ranges::range Range> requires (std::convertible_to<std::ranges::range_value_t<Range>, Vector<T, 2>>)
        ConvexPolygon(Range&& range) noexcept
        {
            auto outline = range | std::views::transform([] (const auto& x) { return Vector<T, 2>(x); });
            mPoints = outline | std::ranges::to<std::vector>();
        }


        template <typename... Args>
        ConvexPolygon(Args&&... args) requires ( std::same_as<std::decay_t<Args>, Vector<T, 2>> && ... )
            : ConvexPolygon(std::array<Vector<T, 2>, sizeof...(Args)>{args...})
        {}

        Vector<T, 2> GetPoint(unsigned int index) const noexcept
        {
            Assert(index < mPoints.size());
            return mPoints[index];
        }

        decltype(auto) Points() const
        {
            return mPoints | std::views::all;
        }

        decltype(auto) PointCount() const noexcept
        {
            return mPoints.size();
        }

        const LineSegment<T, 2> GetLine(unsigned int index) const
        {
            Assert(index < mPoints.size());
            return LineSegment<T, 2>(mPoints[index], mPoints[(index + 1) % mPoints.size()]);
        }


        decltype(auto) LineCount() const
        {
            return mPoints.size();
        }


        Vector<T, 2> CenterOfMass() const
        {
            std::vector<std::pair<double, Vector<T, 2>>> components;

            auto mean = Mean();
            for (int i = 0; i < LineCount(); i++)
            {
                auto line = GetLine(i);

                // Skip degenerate (near-zero-length) edges: they contribute no
                // area to this fan triangulation, and building a Triangle from
                // one can trip its CCW-orientation assertion, since floating
                // point noise can push a true zero-area result to either side
                // of that check.
                if ((line.B() - line.A()).SquareMagnitude() <= 1e-12)
                {
                    continue;
                }

                Triangle<T, 2> tri({line.A(), line.B(), mean});
                components.emplace_back(tri.Area(), tri.GetMean());
            }

            Vector<T, 2> center;
            double totalWeight = 0.0;
            for (const auto& [weight, triMean] : components)
            {
                center = center + (weight * triMean);
                totalWeight += weight;
            }
            return (1.0 / totalWeight) * center;
        }


        Vector<T, 2> Mean() const
        {
            Vector<T, 2> mean;

            for (int i = 0; i < PointCount(); i++)
            {
                mean = mean + GetPoint(i);
            }

            return mean * (1.0 / PointCount());
        }


        [[nodiscard]] AABB<T, 2> GetBoundingBox() const noexcept
        {
            PointSet<T, 2> points;
            for (auto p : mPoints)
            {
                points.Add(p);
            }

            return AABB<T, 2>{points.BoundingBoxMin(), points.BoundingBoxMax()};
        }


        bool Contains(const Vector<T, 2>& v) const noexcept
        {
            auto boundingBox = GetBoundingBox();
            if (!boundingBox.Contains(v))
            {
                return false;
            }

            for (int i = 0; i < LineCount(); i++)
            {
                auto line = GetLine(i);
                if ((line.B() - line.A()).DotPerp(v - line.A()) < 0.0)
                {
                    return false;
                }
            }

            return true;
        }


    private:
        bool IsConvex() const noexcept
        {
            for (int i = 0; i < mPoints.size(); i++)
            {
                const auto& a = mPoints[(i + 0) % mPoints.size()];
                const auto& b = mPoints[(i + 1) % mPoints.size()];
                const auto& c = mPoints[(i + 2) % mPoints.size()];

                if ((b - a).DotPerp(c - a) < 0.0)
                {
                    return false;
                }
            }

            return true;
        }


        std::vector<Vector<T, 2>> mPoints;
    };


    template <typename T>
    struct Translation<ConvexPolygon<T>>
    {
        Translation(auto&& x)
            : parameter(std::forward<decltype(x)>(x))
        {}


        ConvexPolygon<T> operator()(const ConvexPolygon<T>& x) const
        {
            auto transformPoint = [&](auto&& x) { return x + parameter; };
            return ConvexPolygon<T>(x.Points() | std::views::transform(transformPoint));
        }


        Vector<T, 2> parameter;
    };


    template <typename T> requires std::floating_point<T>
    struct Rotation<ConvexPolygon<T>>
    {
        Rotation(auto&& x)
            : parameter(std::forward<decltype(x)>(x))
        {}


        ConvexPolygon<T> operator()(const ConvexPolygon<T>& x) const
        {
            const auto rotMat = Rotate2D(parameter);
            const auto transformPoint =[&](auto&& x) { return rotMat * x; };
            return ConvexPolygon<T>(x.Points() | std::transform(transformPoint));
        }


        double parameter;
    };


    template <typename T>
    struct Scale<ConvexPolygon<T>>
    {
        Scale(auto&& x)
            : parameter(std::forward<decltype(x)>(x))
        {}


        ConvexPolygon<T> operator()(const ConvexPolygon<T>& x) const
        {
            const auto scaleMat = Scale(parameter);
            const auto transformPoint = [&](auto&& x) { return scaleMat * x; };
            return ConvexPolygon<T>(x.Points() | std::views::transform(transformPoint));
        }


        Vector<T, 2> parameter;
    };


    template <typename T>
    struct IntersectionTest<ConvexPolygon<T>, Line<T, 2>>
    {
        using Base = IntersectionTest<LineSegment<T, 2>, Line<T, 2>>::Result::Inner;
        struct Data : Base
        {
            Data(const Base& base)
                : Base(base)
            {}


            unsigned int edgeIndex = 0;
        };

        using Result = std::vector<Data>;

        constexpr Result operator()(const ConvexPolygon<T>& a, const Line<T, 2>& b) const noexcept
        {
            Result result;
            result.reserve(2);

            for (int i = 0; i < a.LineCount(); i++)
            {
                const auto& line = a.GetLine(i);

                if (auto intersection = line.Intersection(b))
                {
                    Data data(intersection.Unwrap());
                    data.edgeIndex = i;
                    result.emplace_back(std::move(data));
                }
            }

            return result;
        }
    };


    template <typename T>
    struct IntersectionTest<ConvexPolygon<T>, Ray<T, 2>>
    {
        using Base = IntersectionTest<LineSegment<T, 2>, Ray<T, 2>>::Result::Inner;
        struct Data : Base
        {
            Data(const Base& base)
                : Base(base)
            {}

            unsigned int edgeIndex = 0;
        };

        using Result = std::vector<Data>;

        constexpr Result operator()(const ConvexPolygon<T>& a, const Ray<T, 2>& b) const noexcept
        {
            Result result;
            result.reserve(2);

            for (int i = 0; i < a.LineCount(); i++)
            {
                const auto& line = a.GetLine(i);
                if (auto intersection = line.Intersection(b))
                {
                    Data data(std::move(intersection.Unwrap()));
                    data.edgeIndex = i;
                    result.emplace_back(std::move(data));
                }
            }

            return result;
        }
    };


    template <typename T>
    struct IntersectionTest<ConvexPolygon<T>, LineSegment<T, 2>>
    {
        using Base = IntersectionTest<LineSegment<T, 2>, LineSegment<T, 2>>::Result::Inner;

        struct Data : Base
        {
            Data(const Base& base)
                : Base(base)
            {}

            unsigned int edgeIndex = 0;
        };

        using Result = std::vector<Data>;

        constexpr Result operator()(const ConvexPolygon<T>& a, const LineSegment<T, 2>& b) const noexcept
        {
            Result result;
            result.reserve(2);

            for (int i = 0; i < a.LineCount(); i++)
            {
                const auto& line = a.GetLine(i);
                if (auto intersection = line.Intersection(b))
                {
                    Data data(std::move(intersection.Unwrap()));
                    data.edgeIndex = i;
                    result.emplace_back(std::move(data));
                }
            }

            return result;
        }
    };
}

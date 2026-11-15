#pragma once


#include "Strawberry/Core/Math/Geometry/Shape.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/Optional.hpp"


namespace Strawberry::Core::Math::Geometry
{
    template <typename T, unsigned int D>
    class Line : public Shape
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

        auto operator<=>(const Line& line) const = default;


        std::string ToString() const noexcept
        {
            return fmt::format("Line({} --> {})", A().ToString(), B().ToString());
        }


    private:
        std::array<Vector<T, D>, 2> mPoints;
    };


    template <typename T, unsigned int D>
    struct Translation<Line<T, D>>
    {
        Translation(auto&& x)
            : parameter(std::forward<decltype(x)>(x))
        {}


        Line<T, D> operator()(const Line<T, D>& x) const
        {
            return Line<T, D>(x.A() + parameter, x.B() + parameter);
        }


        Vector<T, D> parameter;
    };


    template <typename T>
    struct IntersectionTest<Line<T, 2>, Line<T, 2>>
    {
        struct Data
        {
            Line<T, 2> a;
            Line<T, 2> b;
            Vector<T, 2> position;
            double lineDistance[2];
        };

        using Result = Optional<Data>;


        Result operator()(const Line<T, 2>& a, const Line<T, 2>& b) const noexcept
        {
            // Solve equation system using Cramer's rule.
            auto& p1 = a.A();
            auto& p2 = b.A();
            auto   c = p2 - p1;
            auto  v1 = a.Direction();
            auto  v2 = b.Direction();

            double determinant = v1[0] * v2[1] - v2[0] * v1[1];
            double scale = v1.Magnitude() * v2.Magnitude();
            if (scale <= std::numeric_limits<T>::epsilon() ||
                std::abs(determinant) <= std::numeric_limits<T>::epsilon() * scale)
            {
                return NullOpt;
            }

            auto t1Num = c[0] * v2[1] - v2[0] * c[1];
            auto t1 = t1Num / determinant;
            auto t2Num = -(v1[0] * c[1] - c[0] * v1[1]);
            auto t2 = t2Num / determinant;

            return Data
            {
                .a = a,
                .b = b,
                .position = p1 + t1 * v1,
                .lineDistance { t1, t2 },
            };
        }
    };
}


namespace fmt
{
    template <typename T, size_t D>
    struct formatter<Strawberry::Core::Math::Geometry::Line<T, D>> : formatter<std::string>
    {
        using formatter<std::string>::parse;

        auto format(const Strawberry::Core::Math::Geometry::Line<T, D>& l, format_context& ctx) const -> format_context::iterator
        {
            return fmt::format_to(ctx.out(), "Line({} -> {})", l.A(), l.B());
        }
    };
}

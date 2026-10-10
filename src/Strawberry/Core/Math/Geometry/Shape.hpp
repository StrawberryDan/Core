#pragma once


#include "Strawberry/Core/Math/Geometry/Intersection.hpp"
#include "Strawberry/Core/Math/Geometry/Transformation.hpp"
#include <type_traits>


namespace Strawberry::Core::Math::Geometry
{
    class Shape
        : public Intersectable
    {
    public:
        decltype(auto) Translated(this const auto& self, auto&& x)
        {
            return Translation<std::decay_t<decltype(self)>>
                {std::forward<decltype(x)>(x)}(self);
        }


        decltype(auto) Rotated(this const auto& self, auto&& x)
        {
            return Rotation<std::decay_t<decltype(self)>>
                {x}(self);
        }


        decltype(auto) Scaled(this const auto& self, auto&& x)
        {
            return Scale<std::decay_t<decltype(self)>>
                {x}(self);
        }
    };
}


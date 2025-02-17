
#include "Strawberry/Core/Types/TypeSet.hpp"
#include "Strawberry/Core/Types/Variant.hpp"


using namespace Strawberry::Core;


int main()
{
    struct A {};
    struct B {};
    struct C {};
    struct D {};


    static_assert(std::same_as<TypeSet<A, B, C, D>::Into<Variant>, Variant<A, B, C, D>>);


    static_assert(
        TypeSet<A, B>::Union<C>::Equals<
        TypeSet<A, B, C>
    >);

    static_assert(
        TypeSet<A, B>::Union<C>::Union<D>::Equals<
        TypeSet<A, B, C, D>
    >);

    static_assert(
        TypeSet<A, B>::Union<TypeSet<C, D>>::Equals<
        TypeSet<A, B, C, D>>);


    static_assert(
        TypeSet<A, B, C, D>::Intersection<A>::Equals<
            TypeSet<A>>);

    static_assert(
        TypeSet<A, B, C, D>::Intersection<A, B>::Equals<
            TypeSet<A, B>>);

    static_assert(
        TypeSet<C, D>::Intersection<TypeSet<A, C>>::Equals<
            TypeSet<C>>);
}
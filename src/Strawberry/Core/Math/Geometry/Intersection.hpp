#pragma once


#include <type_traits>
#include <compare>


namespace Strawberry::Core::Math
{
	/// Base template delcaration for collision tests.
	template <typename A, typename B>
	struct IntersectionTest;


	/// Concept for checking if two types can be checked for intersection.
	/// I.e. is IntersectionTest<A, B> defined.
	template <typename A, typename B>
	concept IntersectionTestable = requires()
	{
		sizeof(IntersectionTest<A, B>);
	};


	/// Define the commutative property for intersection testing.
	/// I.e. if A and B can be checked for intersection then B and A can be checked also.
	template <typename A, typename B> requires requires { sizeof(IntersectionTest<B, A>); }
	struct IntersectionTest<A, B>
	{
		using Result = IntersectionTest<B, A>::Result;

		constexpr Result operator()(const A& a, const B& b) const noexcept
		{
			return IntersectionTest<B, A>{}(b, a);
		}
	};


	/// Base class for types supporting intersection operations.
	/// Provides method syntax for intersectiont testing.
	class Intersectable
	{
	public:
		/// Make default comparable so that derriving classes are also orderable.
		auto operator<=>(const Intersectable& other) const noexcept = default;


		/// Intersection method.
		auto Intersection(this const auto& self, const auto& other) noexcept
		{
			using TestType = IntersectionTest<
				std::decay_t<decltype(self)>,
				std::decay_t<decltype(other)>>;

			return TestType{}(self, other);
		}
	};
}

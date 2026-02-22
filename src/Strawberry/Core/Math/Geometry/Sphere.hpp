#pragma once

#include "Strawberry/Core/Math/Vector.hpp"
#include "fmt/format.h"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned D>
	class Sphere
	{
	public:
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

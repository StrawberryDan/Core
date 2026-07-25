#pragma once

#include "Strawberry/Core/IO/Logging.hpp"
#include "Strawberry/Core/Math/Geometry/Intersection.hpp"
#include "Strawberry/Core/Math/Vector.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
#include "Strawberry/Core/Math/Geometry/AABB.hpp"
#include "fmt/format.h"
#include <limits>


namespace Strawberry::Core::Math
{
	template <typename T, unsigned int D>
	class AABB;


	template <typename T, unsigned D = 3>
	class Sphere
		: public Intersectable
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


		auto operator<=>(const Sphere& other) const noexcept = default;


		Sphere Translated(const Vector<T, D>& translation) const
		{
			return Sphere(mCenter + translation, mRadius);
		}


	private:
		Vector<T, D> mCenter;
		T            mRadius;
	};


	template <typename T>
	using Circle = Sphere<T, 2>;


	template <typename T, unsigned D>
	struct IntersectionTest<Sphere<T, D>, Sphere<T, D>>
	{
		struct Data
		{
			const Sphere<T, D> a;
			const Sphere<T, D> b;
		};

		using Result = Core::Optional<Data>;


		Result operator()(const Sphere<T, D>& a, const Sphere<T, D>& b) noexcept
		{
			bool collided = (a.Center() - b.Center()).SquareMagnitude() < std::pow(a.Radius() + b.Radius(), 2);
			if (!collided)
			{
				return NullOpt;
			}

			return Data {
				.a = a,
				.b = b,
			};
		}
	};


	template <typename T, unsigned D>
	struct MTVCalculator<Sphere<T, D>, Sphere<T, D>>
	{
		using Result = Vector<T, D>;


		Result operator()(const Sphere<T, D>& a, const Sphere<T, D>& b) noexcept
		{
			auto vectorBetweenCenters = (b.Center() - a.Center());
			auto distanceBetweenCenters = vectorBetweenCenters.Magnitude();

			auto penetrationDepth = (a.Radius() + b.Radius()) - distanceBetweenCenters;

			return vectorBetweenCenters.WithLength(penetrationDepth);
		}
	};


	template <typename T, unsigned D>
	struct IntersectionTest<AABB<T, D>, Sphere<T, D>>
	{
		struct Data
		{
			const AABB<T, D>  a;
			const Sphere<T, D> b;
		};

		using Result = Core::Optional<Data>;


		Result operator()(const AABB<T, D>& a, const Sphere<T, D>& b) noexcept
		{
			auto clamped = a.Clamp(b.Center());

			auto distance = (clamped - b.Center()).SquareMagnitude();

			bool collision = distance <= (b.Radius() * b.Radius());
			if (!collision)
			{
				return NullOpt;
			}

			return Data
			{
				.a = a,
				.b = b,
			};
		}
	};


	template <typename T, unsigned int D>
	struct MTVCalculator<AABB<T, D>, Sphere<T, D>>
	{
		using Result = Vector<T, D>;


		T GetPenetrationDepth(const AABB<T, D>& a, const Sphere<T, D> b, unsigned int d) const
		{
			auto c = b.Center()[d];

			auto min = a.Min()[d];
			auto max = a.Max()[d];

			auto mid = 0.5 * (min + max);

			if (c < mid)
			{
				return c - min;
			}
			else
			{
				return max - c;
			}
		}


		Result operator()(const AABB<T, D>& a, const Sphere<T, D>& b) noexcept
		{
			if (a.Contains(b.Center())) [[unlikely]]
			{
				unsigned int minPenetrationDimension = 0;
				T minPenetrationDepth = std::numeric_limits<T>::max();

				for (int d = 0; d < D; d++)
				{
					T penetrationDepth = GetPenetrationDepth(a, b, d);
					if (penetrationDepth < minPenetrationDepth)
					{
						minPenetrationDepth = penetrationDepth;
						minPenetrationDimension = d;
					}
				}

				Vector<T, D> MTV;
				auto centerDist = b.Center()[minPenetrationDimension] - a.Center()[minPenetrationDimension];
				MTV[minPenetrationDimension] = minPenetrationDepth;
				if (centerDist < 0) MTV[minPenetrationDimension] = MTV[minPenetrationDimension] * -1;
				return MTV;
			}
			else
			{
				auto closestPoint = a.Clamp(b.Center());

				auto distance = (b.Center() - closestPoint).Magnitude();

				return (closestPoint - b.Center()).WithLength(b.Radius() - distance);
			}
		}
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

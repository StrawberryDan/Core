#pragma once


#include "Strawberry/Core/Math/Vector.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned int D>
	class Line;
	template <typename T, unsigned int D>
	class LineSegment;
	template <typename T, unsigned int D>
	class Ray;


	template <typename T, unsigned D>
	class Ray
	{
	public:
		Ray() = default;
		Ray(const Vector<T, D>& origin, const Vector<T, D>& direction)
			: mOrigin(origin), mDirection(direction.Normalised()) {}


		Vector<T, D>& Origin() { return mOrigin; }
		const Vector<T, D>& Origin() const { return mOrigin; }


		void SetDirection(const Vector<T, D>& direction) { mDirection = direction.Normalised(); }
		const Vector<T, D>& Direction() const { return mDirection; }


		Line<T, D> IntoLine() const
		{
			return Line(mOrigin, mOrigin + mDirection);
		}


		Optional<Vector<T, D>> Intersection(const Ray& other) const noexcept
		{
			// Solve using Cramer's rule
			auto& p1 = this->Origin();
			auto& p2 = other.Origin();
			auto   c = p2 - p1;
			auto& v1 = this->Direction();
			auto& v2 = other.Direction();

			double determinant = v1[0] * v2[1] - v2[0] - v1[1];
			if (determinant == 0.0)
			{
				return NullOpt;
			}

			auto t1Num = c[0] * v2[1] - v2[0] * c[1];
			auto t1 = t1Num / determinant;
			auto t2Num = v1[0] * c[1] - c[0] * v1[1];
			auto t2 = t2Num / determinant;

			if (t1 < 0.0 || t2 < 0.0)
			{
				return NullOpt;
			}

			return p1 + t1 * v1;
		}


	private:
		Vector<T, D> mOrigin;
		Vector<T, D> mDirection;
	};


	template <typename T, unsigned int D>
	class Line
	{
	public:
		Line() = default;
		Line(const Vector<T, D> a, const Vector<T, D>& b)
			: mPoints{a, b} {}


		Vector<T, D>& A() { return mPoints[0]; }
		const Vector<T, D>& A() const { return mPoints[0]; }
		Vector<T, D>& B() { return mPoints[1]; }
		const Vector<T, D>& B() const { return mPoints[1]; }



	private:
		Vector<T, D> mPoints[2];
	};


	template <typename T, unsigned D>
	class LineSegment : private Line<T, D>
	{
	public:
		using Line<T, D>::Line;


		using Line<T, D>::A;
		using Line<T, D>::B;
namespace fmt
{
	template <typename T, unsigned D>
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

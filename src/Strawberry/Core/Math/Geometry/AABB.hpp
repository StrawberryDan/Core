#pragma once


#include "Strawberry/Core/Math/Vector.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned int D>
	class AABB
	{
	public:
		static AABB FromOriginAndExtent(const Vector<T, D>& origin, const Vector<T, D>& extent)
		{
			return AABB(origin, origin + extent);
		}

		AABB(const Vector<T, D>& min, const Vector<T, D>& max)
			: mMin(min)
			, mMax(max)
		{}


		AABB() = default;


		const auto&  Min() const { return mMin; }
		const auto&  Max() const { return mMax; }
		auto        Size() const { return Max() - Min(); }
		auto        Center() const { return (Min() + Max()).Piecewise(std::divides{}, 2); }

		void SetMin(const Vector<T, D>& min) noexcept { mMin = min; }
		void SetMax(const Vector<T, D>& max) noexcept { mMax = max; }


		bool IsNormal() const
		{
			for (unsigned int i = 0; i < D; i++)
			{
				if (mMin[i] > mMax[i])
				{
					return false;
				}
			}
			return true;
		}


		void Normalise()
		{
			for (int i = 0; i < D; i++)
			{
				if (mMin[i] > mMax[i]) [[unlikely]] std::swap(mMin[i], mMax[i]);
			}
		}

	private:
		Vector<T, D> mMin;
		Vector<T, D> mMax;
	};
}
#pragma


#include "Strawberry/Core/Math/Vector.hpp"
#include <algorithm>


namespace Strawberry::Core::Math
{
	template <typename T, unsigned D>
	class PointSet
	{
	public:
		PointSet() = default;


		const Vector<T, D> Get(unsigned int i) const { return mPoints[i]; }
		Vector<T, D>& Get(unsigned int i) { return mPoints[i]; }


		unsigned int Size() const { return mPoints.size(); }


		void Add(const Vector<T, D>& point)
		{
			mPoints.push_back(point);
		}


		Vector<T, D> MinExtreme() const
		{
			Vector<T, D> min;
			for (auto point : mPoints)
			{
				for (int d = 0; d < D; d++)
				{
					min[d] = std::min(min[d], point[d]);
				}
			}
			return min;
		}


		Vector<T, D> MaxExtreme() const
		{
			Vector<T, D> max;
			for (auto point : mPoints)
			{
				for (int d = 0; d < D; d++)
				{
					max[d] = std::max(max[d], point[d]);
				}
			}
			return max;
		}


	private:
		std::vector<Vector<T, D>> mPoints;
	};
}

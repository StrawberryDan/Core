#pragma


#include "Strawberry/Core/Math/Vector.hpp"


namespace Strawberry::Core::Math
{
	template <typename T, unsigned D>
	class PointSet
	{
	public:
		PointSet() = default;


		void Add(const Vector<T, D>& point)
		{
			mPoints.push_back(point);
		}


	private:
		std::vector<Vector<T, D>> mPoints;
	};
}

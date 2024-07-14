#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Math/Vector.hpp"
// Standard Library
#include <concepts>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core::Math
{
	template<typename T = double, unsigned int D = 2> requires (std::integral<T> || std::floating_point<T>)
	class AABB
	{
	public:
		using VectorType = Vector<T, D>;

	public:
		AABB(VectorType position, VectorType extent);


		VectorType& Position()
		{
			return mPosition;
		}


		const VectorType& Position() const
		{
			return mPosition;
		}


		VectorType& Extent()
		{
			return mExtent;
		}


		const VectorType& Extent() const
		{
			return mExtent;
		}


		T Mensuration() const
		{
			T result = mExtent[0];
			for (int i = 1; i < D; ++i) result *= mExtent[i];
			return result;
		}


		T Area() const requires (D == 2)
		{
			return Mensuration();
		}


		T Volume() const requires (D == 3)
		{
			return Mensuration();
		}


		bool Intersects(const AABB& other)
		{
			for (int i = 0; i < D; ++i)
			{
				const bool a = mPosition[i] >= other.Position()[i] && mPosition[i] <= other.Position()[i] + other.Extent()[i];
				const bool b = other.Position()[i] >= Position()[i] && other.mPosition[i] <= Position()[i] + Extent()[i];
				if (!a && !b) return false;
			}

			return true;
		}

	private:
		VectorType mPosition;
		VectorType mExtent;
	};


	using AABB2  = AABB<double, 2>;
	using AABB2f = AABB<float, 2>;
	using AABB2i = AABB<int, 2>;
	using AABB2u = AABB<unsigned int, 2>;


	using AABB3  = AABB<double, 2>;
	using AABB3f = AABB<float, 2>;
	using AABB3i = AABB<int, 2>;
	using AABB3u = AABB<unsigned int, 2>;
}

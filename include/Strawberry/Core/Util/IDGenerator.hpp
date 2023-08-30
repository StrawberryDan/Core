#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <algorithm>
#include <concepts>
#include <set>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template <std::integral T = size_t>
	class IDGenerator
	{
	public:
		T Generate()
		{
			if (mFreeIDs.empty()) { return mGreatestID++; }
			else
			{
				auto minimum = std::min_element(mFreeIDs.begin(), mFreeIDs.end());
				mFreeIDs.erase(minimum);
				return minimum;
			}
		}

		void Free(T id) { mFreeIDs.emplace(id); }


	private:
		T           mGreatestID = 0;
		std::set<T> mFreeIDs;
	};
} // namespace Strawberry::Core
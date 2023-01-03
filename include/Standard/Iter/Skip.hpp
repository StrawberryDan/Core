#pragma once



#include "Standard/Option.hpp"
#include "Standard/Concepts.hpp"



namespace Strawberry::Standard::Iter
{
	template <typename T>
	class Iterator;



	template <typename T>
	class Skipped : public Iterator<T>
	{
	public:
		Option<T> Next() override
		{
			while (mSkipCount > 0)
			{
				mBase->Next();
				--mSkipCount;
			}

			return mBase->Next();
		}
	private:
		template <typename>
		friend class Iterator;


		Skipped(Iterator<T>* base, unsigned int dropCount)
				: mBase(base)
				, mSkipCount(dropCount)
		{}



		unsigned int mSkipCount;
		Iterator<T>* mBase;
	};
}
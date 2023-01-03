#pragma once



#include "Standard/Concepts.hpp"
#include "Standard/Option.hpp"



namespace Strawberry::Standard::Iter
{
	template <typename T>
	class Iterator;



	template<typename T, Callable<bool, T> Predicate>
	class Filtered : public Iterator<T>
	{
	public:
		Option<T> Next() override
		{
			while (true)
			{
				auto value = mBase->Next();
				if (!value)
				{
					return value;
				}

				if (mPredicate(*value))
				{
					return value;
				}
			}
		}

	private:
		template <typename>
		friend class Iterator;

		Filtered(Iterator<T>* base, Predicate predicate)
				: mBase(base)
				, mPredicate(predicate)
		{}



		Iterator<T>* mBase;
		Predicate    mPredicate;
	};
}

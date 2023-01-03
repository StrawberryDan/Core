#pragma once



#include "Standard/Concepts.hpp"
#include "Standard/Option.hpp"



#include "Standard/Iter/Filter.hpp"
#include "Standard/Iter/Map.hpp"
#include "Standard/Iter/Skip.hpp"



namespace Strawberry::Standard::Iter
{
	template<typename T>
	class Iterator
	{
	public:
		virtual Option<T> Next() = 0;



		virtual Option<size_t> Size()
		{
			return {};
		}



		template<typename R, Callable<R, T> Functor>
		Mapped<R, T, Functor> Map(Functor functor)
		{
			return Mapped<R, T, Functor>(this, functor);
		}



		template <Callable<bool, T> Predicate>
		Filtered<T, Predicate> Filter(Predicate predicate)
		{
			return Filtered<T, Predicate>(this, predicate);
		}



		Skipped<T> Drop(unsigned int count)
		{
			return Skipped<T>(this, count);
		}



		template <STLIteratorConstructible<T> Container>
		Container Collect()
		{
			std::vector<T> mValues;
			if (auto size = Size())
			{
				mValues.reserve(*size);
			}
			while (auto v = this->Next())
			{
				mValues.push_back(*v);
			}
			return Container(mValues.begin(), mValues.end());
		}
	};
}
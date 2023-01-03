#pragma once



#include "Standard/Option.hpp"
#include "Standard/Concepts.hpp"



namespace Strawberry::Standard::Iter
{
	template <typename T>
	class Iterator;



	template<typename R, typename V, Callable<R, V> Functor>
	class Mapped : public Iterator<R>
	{
	public:
		Option<R> Next() override
		{
			if (Option<V> value = mBase->Next())
			{
				return value.template Map<R>(mFunctor);
			}
			else
			{
				return {};
			}
		}

	private:
		template <typename>
		friend class Iterator;

		Mapped(Iterator<V>* base, Functor functor)
				: mBase(base)
				, mFunctor(functor)
		{}



		Iterator<V>* mBase;
		Functor      mFunctor;
	};
}
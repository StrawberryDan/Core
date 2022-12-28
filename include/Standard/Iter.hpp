#pragma once



#include <Standard/Option.hpp>




#include <concepts>
#include <iterator>




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



	template <typename T>
	class Dropped : public Iterator<T>
	{
	public:
		Option<T> Next() override
		{
			while (mDropCount > 0)
			{
				mBase->Next();
				mDropCount--;
			}

			return mBase->Next();
		}
	private:
		template <typename>
		friend class Iterator;


		Dropped(Iterator<T>* base, unsigned int dropCount)
			: mBase(base)
			, mDropCount(dropCount)
		{}



		unsigned int mDropCount;
		Iterator<T>* mBase;
	};



	template<typename T>
	class Iterator
	{
	public:
		virtual Option<T> Next() = 0;



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



		Dropped<T> Drop(unsigned int count)
		{
			return Dropped<T>(this, count);
		}
	};



	template <typename T>
	concept STLIterable = requires(T t)
	{
		requires std::forward_iterator<typename T::iterator>;
		{ t.begin() } -> std::forward_iterator;
		{ t.end()   } -> std::forward_iterator;
	};



	template <STLIterable Container, typename V = std::remove_reference_t<decltype(*std::declval<Container>().begin())>>
	class STLWrapper : public Iterator<V>
	{
	public:
		STLWrapper(Container& container)
			: mContainer(&container)
			, mPosition(container.begin())
		{}



		virtual Option<V> Next() override
		{
			if (mPosition == mContainer->end())
			{
				return {};
			}
			else
			{
				Option<V> value = *mPosition;
				mPosition++;
				return value;
			}
		}

	private:
		Container*                   mContainer;
		typename Container::iterator mPosition;
	};
}
#pragma once



#include "Standard/Concepts.hpp"
#include "Standard/Iter/Iterator.hpp"
#include "Standard/Option.hpp"




#include <concepts>
#include <iterator>




namespace Strawberry::Standard::Iter
{
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
				return *mPosition++;
			}
		}



		virtual Option<size_t> Size() override
		{
			return mContainer->size();
		}



	private:
		Container*                   mContainer;
		typename Container::iterator mPosition;
	};
}
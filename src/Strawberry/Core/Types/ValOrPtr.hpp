#pragma once
#include "Variant.hpp"


namespace Strawberry::Core
{
	template <typename T>
	class ValOrPtr
		: public Variant<T, T*>
	{
	public:
		using Variant<T, T*>::Variant;


		const T& Resolve() const
		{
			return this->Visit(Overload(
				[] (const T& t) -> const T& { return  t;  },
				[] (const T* t) -> const T& { return *t; }
			));
		}


		T& Resolve()
		{
			return this->Visit(Overload(
				[] (T& t) -> T& { return  t;  },
				[] (T* t) -> T& { return *t; }
			));
		}
	};
}

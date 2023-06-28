#pragma once



#include <concepts>
#include <iterator>
#include <vector>



namespace Strawberry::Core
{
	template <typename T, typename V>
	concept Indexable = requires(T t, size_t i)
	{
		{ t[i] } -> std::same_as<V>;
	};



	template <typename F, typename R, typename ...Args>
	concept Callable = requires
	{
	    requires std::invocable<F, Args...>;
	    requires std::is_same_v<R, std::invoke_result_t<F, Args...>>;
	};
}

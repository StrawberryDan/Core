#pragma once



#include <concepts>



namespace Strawberry::Standard
{
	template <typename F, typename R, typename ...Args>
	concept Callable = requires
	{
	    requires std::invocable<F, Args...>;
	    requires std::is_same_v<R, std::invoke_result_t<F, Args...>>;
	};



	template <typename T>
	concept STLIterable = requires(T t)
	{
		requires std::forward_iterator<typename T::iterator>;
		{ t.begin() } -> std::forward_iterator;
		{ t.end()   } -> std::forward_iterator;
		{ t.size()  } -> std::same_as<size_t>;
	};



	template <typename T, typename V>
	concept STLIteratorConstructible = requires (T t)
	{
		requires std::constructible_from<T, decltype(std::declval<std::vector<V>>().begin()), decltype(std::declval<std::vector<V>>().end())>;
	};
}

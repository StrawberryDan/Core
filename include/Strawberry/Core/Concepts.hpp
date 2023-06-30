#pragma once



#include <concepts>
#include <iterator>
#include <vector>



namespace Strawberry::Core
{
	template <typename T, typename V>
	concept ValueIndexable = requires(T t, size_t i)
	{
		{ t[i] } -> std::same_as<V>;
	};



	template <typename T, typename V>
	concept ReferenceIndexable = requires(T t, size_t i)
	{
		{ t[i] } -> std::same_as<V&>;
	};



	template <typename T, typename V>
	concept ConstIndexable = requires(const T t, size_t i)
	{
		{ t[i] } -> std::same_as<const V&>;
	};



	template <typename T, typename V>
	concept Indexable = ValueIndexable<T, V> || ReferenceIndexable<T, V> || ConstIndexable<T, V>;



	template <typename T> requires Indexable<T, std::decay_t<decltype(std::declval<T>()[std::declval<size_t>()])>>
	using IndexedType = std::decay_t<decltype(std::declval<T>()[std::declval<size_t>()])>;



	template <typename T>
	concept SizedContainer = requires(T t)
	{
		{ t.Size() } -> std::same_as<size_t>;
	};


	template <typename T>
	concept ResizableContainer = requires(T t, size_t i)
	{
		{ t.Resize(i) } -> std::same_as<void>;
	};



	template <typename F, typename R, typename ...Args>
	concept Callable = requires
	{
	    requires std::invocable<F, Args...>;
	    requires std::is_same_v<R, std::invoke_result_t<F, Args...>>;
	};
}

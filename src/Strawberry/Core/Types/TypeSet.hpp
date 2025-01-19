//======================================================================================================================
//		Type Set
//
//		Class for representing sets of types and operations on those sets.
//----------------------------------------------------------------------------------------------------------------------
#pragma once


namespace Strawberry::Core
{
	template <typename... Ts>
	struct TypeSet
	{
		// Transforms this set into a type with matching arg parameters.
		template <template<typename...> typename T>
		using Into = T<Ts...>;


		// The number of types in this set
		static constexpr size_t Size = sizeof...(Ts);


		// Whether this set contains the given types.
		template <typename...>
		struct _Contains
		{
			static constexpr bool Value = false;
		};

		template <typename... Args>
		static constexpr bool Contains = _Contains<Args...>::Value;

		template <typename T> requires(std::same_as<T, Ts> || ...)
		struct _Contains<T>
		{
			static constexpr bool Value = true;
		};


		// Whether this set is equivalent to another.
		template <typename...>
		struct _Equals {};

		template <typename... Args>
		static constexpr bool Equals = _Equals<Args...>::Value;

		template <typename... Args>
		struct _Equals<TypeSet<Args...>>
		{
			static constexpr bool Value = false;
		};

		template <typename... Args> requires (sizeof...(Args) == sizeof...(Ts) && (Contains<Args> && ...))
		struct _Equals<TypeSet<Args...>>
		{
			static constexpr bool Value = true;
		};


		// Returns a typeset containing only the first type in this set.
		struct _Head
		{
			template <typename...>
			struct First {};

			template <typename T, typename... Args>
			struct First<T, Args...> { using Type = TypeSet<T>; };

			template <typename... Args> requires (sizeof...(Args) == 0)
			struct First<Args...> { using Type = TypeSet<>; };

			using Type = First<Ts...>::Type;
		};

		using Head = typename _Head::Type;


		// Returns a typeset containing all but the first type in this set.
		struct _Tail
		{
			template <typename...>
			struct Last {};

			template <typename T, typename... Args>
			struct Last<T, Args...> { using Type = TypeSet<Args...>; };

			template <typename... Args> requires (sizeof...(Args) == 0)
			struct Last<Args...> { using Type = TypeSet<>; };

			using Type = typename Last<Ts...>::Type;
		};

		using Tail = typename _Head::Type;


		// Returns a typeset containing the union of all types between this set and the arguments passed.
		template <typename...>
		struct _Union {};

		template <typename... Types>
		using Union = typename _Union<Types...>::Type;

		template <typename... Types> requires (sizeof...(Types) == 0)
		struct _Union<Types...>
		{
			using Type = TypeSet;
		};

		template <typename... Types> requires (sizeof...(Ts) == 0)
		struct _Union<Types...>
		{
			using Type = TypeSet<Types...>;
		};

		template <typename... Types> requires (sizeof...(Types) == 0 && sizeof...(Ts) == 0)
		struct _Union<Types...>
		{
			using Type = TypeSet<>;
		};

		template <typename T, typename... Rest> requires (!Contains<T>)
		struct _Union<T, Rest...>
		{
			using Type = typename TypeSet<Ts..., T>::template Union<Rest...>;
		};

		template <typename T, typename... Rest> requires (Contains<T>)
		struct _Union<T, Rest...>
		{
			using Type = Union<Rest...>;
		};


		template <typename... Types>
		struct _Union<TypeSet<Types...>>
		{
			using Type = Union<Types...>;
		};


		// Returns a type set containing the intersection of this set and the arguments.
		template <typename...>
		struct _Intersection {};

		template <typename... Args>
		using Intersection = typename _Intersection<Args...>::Type;

		template <typename... Args> requires (sizeof...(Args) == 0 || sizeof...(Ts) == 0)
		struct _Intersection<Args...>
		{
			using Type = TypeSet<>;
		};

		template <typename T, typename... Rest> requires (!Contains<T>)
		struct _Intersection<T, Rest...>
		{
			using Type = Intersection<Rest...>;
		};

		template <typename T, typename... Rest> requires (Contains<T>)
		struct _Intersection<T, Rest...>
		{
			using Type = typename TypeSet<T>::template Union<Intersection<Rest...>>;
		};

		template <typename... Args>
		struct _Intersection<TypeSet<Args...>>
		{
			using Type = Intersection<Args...>;
		};
	};
}

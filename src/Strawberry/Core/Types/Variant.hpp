#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Optional.hpp"
#include "Overload.hpp"
// Standard Library
#include <concepts>
#include <functional>


//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	// Type Aliases
	using TypeIndex = size_t;
	static constexpr TypeIndex TypeNotFound = std::numeric_limits<TypeIndex>::max();

	// Predeclarations
	//
	// Variant type
	template<typename... Ts>
	class Variant;
	// Helper struct which calculates the variant type that is the union of the types given.
	template<typename V1, typename... V2>
	struct VariantUnion {};
	template <typename V>
	struct VariantSize {};
	template <typename V>
	struct VariantAlignment {};

	template <TypeIndex, typename...>
	struct VariantTypeIndexImpl {};

	template <TypeIndex I, typename Subject, typename A> requires (!std::same_as<Subject, A>)
	struct VariantTypeIndexImpl<I, Subject, A>
	{
		static constexpr TypeIndex Value = TypeNotFound;
	};

	template <TypeIndex I, typename Subject, typename A> requires (std::same_as<Subject, A>)
	struct VariantTypeIndexImpl<I, Subject, A>
	{
		static constexpr TypeIndex Value = I;
	};

	template <TypeIndex I, typename Subject, typename A, typename... Ts> requires(!std::same_as<Subject, A>)
	struct VariantTypeIndexImpl<I, Subject, A, Ts...>
	{
		static constexpr TypeIndex Value = VariantTypeIndexImpl<I + 1, Subject, Ts...>::Value;
	};

	template <TypeIndex I, typename Subject, typename A, typename... Ts> requires std::same_as<Subject, A>
	struct VariantTypeIndexImpl<I, Subject, A, Ts...>
	{
		static constexpr TypeIndex Value = I;
	};

	template <typename Subject, typename... Ts>
	using VariantTypeIndex =  VariantTypeIndexImpl<0, std::decay_t<Subject>, std::decay_t<Ts>...>;



	// Helper Struct Implementations
	//
	// VariantUnion implementation for one variant on the left
	template<typename... Left, typename... Right>
	struct VariantUnion<Variant<Left...>, Right...>
	{
		using Type = Variant<Left..., Right...>;
	};


	// Variant Union Implementation for 2 variants.
	template<typename... Left, typename... Right>
	struct VariantUnion<Variant<Left...>, Variant<Right...>>
	{
		using Type = Variant<Left..., Right...>;
	};


	template <typename... Ts>
	struct VariantSize<Variant<Ts...>>
	{
		static constexpr TypeIndex Value = std::max({sizeof(Ts)...});
	};


	template <typename... Ts>
	struct VariantAlignment<Variant<Ts...>>
	{
		static constexpr TypeIndex Value = std::max({alignof(Ts)...});
	};


	// Helper struct calculating whether a type is part of a variant.
	template<typename T, typename... Ts>
	struct IsInVariant
			: std::false_type {};


	// Specialization of IsInVariant for when a type is inside a variant.
	template<typename T, typename... Ts> requires (std::same_as<std::decay_t<T>, Ts> || ...)
	struct IsInVariant<T, Variant<Ts...>>
			: std::true_type {};





	// Variant Class Declaration
	template<typename... Ts>
	class Variant
	{
	public:
		template<typename... Additional>
		using Union = typename VariantUnion<Variant, Additional...>::Type;


		Variant()
			: mTypeIndex(std::numeric_limits<size_t>::max())
		{}


		template<typename Arg> requires IsInVariant<std::decay_t<Arg>, Variant>::value
		Variant(Arg&& arg)
			: mTypeIndex(VariantTypeIndex<Arg, Ts...>::Value)
		{
			std::construct_at(reinterpret_cast<std::decay_t<Arg>*>(mData), std::forward<Arg>(arg));
		}


		Variant(const Variant& x) requires (... && std::is_copy_constructible_v<Ts>)
		{
			mTypeIndex = x.mTypeIndex;
			x.Visit([this](auto&& value)
			{
				using T = std::decay_t<decltype(value)>;
				std::construct_at<T>(reinterpret_cast<T*>(mData), value);
			});
		}

		Variant& operator=(const Variant& x) requires (... && std::is_copy_constructible_v<Ts>)
		{
			if (this != &x)
			{
				std::destroy_at(this);
				std::construct_at(this, x);
			}

			return *this;
		}


		Variant(Variant&& x) noexcept requires (... && std::is_move_constructible_v<Ts>)
		{
			mTypeIndex = x.mTypeIndex;
			x.Visit([this](auto&& x)
			{
				using T = std::decay_t<decltype(x)>;
				std::construct_at<std::decay_t<decltype(x)>>(reinterpret_cast<T*>(mData), std::move(x));
			});
		}


		Variant& operator=(Variant&& x) noexcept requires (... && std::is_move_constructible_v<Ts>)
		{
			if (this != &x)
			{
				std::destroy_at(this);
				std::construct_at(this, std::move(x));
			}

			return *this;
		}


		~Variant()
		{
			if (mTypeIndex != std::numeric_limits<size_t>::max())
			{
				Visit([](auto&& x)
				{
					std::destroy_at(&x);
				});
			}
		}


		template<typename Arg>
		[[nodiscard]] bool IsType() const
		{
			return mTypeIndex == VariantTypeIndex<Arg, Ts...>::Value;
		}


		template<typename Arg>
		Optional<Arg> Take()
		{
			if (IsType<Arg>()) [[likely]]
			{
				return std::move(*reinterpret_cast<Arg*>(mData));
			}
			return NullOpt;
		}


		template<typename Arg>
		const Arg& Ref() const
		{
			Core::Assert(IsType<Arg>());
			return *reinterpret_cast<const Arg*>(mData);
		}


		template<typename Arg>
		Optional<Arg*> Ptr() &
		{
			if (IsType<Arg>())
			{
				return reinterpret_cast<Arg*>(mData);
			}

			return NullOpt;
		}


		template<typename Arg>
		Optional<const Arg*> Ptr() const &
		{
			if (IsType<Arg>())
			{
				return reinterpret_cast<const Arg*>(mData);
			}

			return NullOpt;
		}


		template <typename F>
		decltype(auto) Visit(this auto&& self, F&& functor)
		{
			using Arg = std::tuple_element<0, std::tuple<Ts...>>::type;

			if constexpr (std::same_as<const Variant&, decltype(self)>)
			{
				using Ret = std::invoke_result_t<F, const Arg&>;

				using FN  = Ret(*)(const void*, F&&);
				static constexpr FN sDispatchTable[] = { [](const void* data, F&& functor) -> Ret
				{
					return std::invoke(std::forward<F>(functor), *static_cast<const Ts*>(data));
				}... };

				return std::invoke(sDispatchTable[self.mTypeIndex], reinterpret_cast<const void*>(self.mData), std::forward<F>(functor));
			}
			else
			{
				using Ret = std::invoke_result_t<F, Arg&>;

				using FN  = Ret(*)(void*, F&&);
				static constexpr FN sDispatchTable[] = { [](void* data, F&& functor) -> Ret
				{
					return std::invoke(std::forward<F>(functor), *static_cast<Ts*>(data));
				}... };

				return std::invoke(sDispatchTable[self.mTypeIndex], reinterpret_cast<void*>(self.mData), std::forward<F>(functor));
			}
		}


		template <typename... Fs> requires (sizeof...(Fs) > 1)
		decltype(auto) Visit(this auto&& self, Fs&&... fs)
		{
			return self.Visit(Overload(std::forward<Fs>(fs)...));
		}


		template <typename T>
		bool operator==(const T& other) const noexcept
		{
			using Fn = bool(*)(const void*, const void*);

			static constexpr Fn fns[] = {
				[](const void* pA, const void* pB)
				{
					const Ts& a = *static_cast<const Ts*>(pA);
					const T&  b = *static_cast<const T* >(pB);

					if constexpr (std::equality_comparable_with<Ts, T>)
					{
						return a == b;
					}

					return false;
				}...
			};

			return std::invoke(fns[mTypeIndex], mData, &other);
		}


		bool operator==(const Variant& other) const noexcept requires (std::equality_comparable<Ts> && ...)
		{
			using Fn = bool(*)(const void*, const void*);

			static constexpr Fn fns[] = { [](const void* a, const void* b)
			{
				return *static_cast<const Ts*>(a) == *static_cast<const Ts*>(b);
			}... };

			return std::invoke(fns[mTypeIndex], mData, other.mData);
		}


		template <typename T>
		bool operator!=(const T& other) const noexcept
		{
			using Fn = bool(*)(const void*, const void*);

			static constexpr Fn fns[] = {
				[](const void* pA, const void* pB)
				{
					const Ts& a = *static_cast<const Ts*>(pA);
					const T&  b = *static_cast<const T* >(pB);

					if constexpr (std::equality_comparable_with<Ts, T>)
					{
						return a != b;
					}

					return true;
				}...
			};

			return std::invoke(fns[mTypeIndex], mData, &other);
		}


		bool operator!=(const Variant& other) const noexcept requires (std::equality_comparable<Ts> && ...)
		{
			using Fn = bool(*)(const void*, const void*);

			static constexpr Fn fns[] = { [](const void* a, const void* b)
			{
				return *static_cast<const Ts*>(a) != *static_cast<const Ts*>(b);
			}... };

			return std::invoke(fns[mTypeIndex], mData, other.mData);
		}


	private:
		static constexpr size_t Size = VariantSize<Variant>::Value;
		static constexpr size_t Alignment = VariantAlignment<Variant>::Value;


		size_t                     mTypeIndex;
		alignas(Alignment) uint8_t mData[Size] = { 0 };
	};
} // namespace Strawberry::Core

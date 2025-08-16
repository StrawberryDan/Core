#pragma once


#include "Strawberry/Core/Assert.hpp"
#include "Optional.hpp"
#include <variant>

#include "Strawberry/Core/Types/Variant.hpp"


namespace Strawberry::Core
{
	struct Success_t {};
	static constexpr Success_t Success{};


	template<typename D, typename E> requires (!std::same_as<D, E>)
	class [[nodiscard]] Result
	{
	public:
		template <typename T>
		Result(T&& t)
		{
			if constexpr (std::same_as<D, std::decay_t<T>> || (std::constructible_from<D, decltype(t)> && !std::constructible_from<E, decltype(t)>))
			{
				mPayload = D(std::forward<T>(t));
			}
			else if constexpr (std::same_as<E, std::decay_t<T>> || (std::constructible_from<E, decltype(t)> && !std::constructible_from<D, decltype(t)>))
			{
				mPayload = E(std::forward<T>(t));
			}
			else if constexpr (std::constructible_from<D, decltype(t)> && std::constructible_from<E, decltype(t)>)
			{
				static_assert(false, "Ambiguous construction as D and E can be constructed from these args");
			}
			else
			{
				static_assert(false, "Could not construct either D or E from this argument");
			}
		}


		[[nodiscard]] inline bool IsOk() const
		{
			return mPayload.template IsType<D>();
		}


		[[nodiscard]] inline bool IsErr() const
		{
			return mPayload.template IsType<E>();
		}


		const D& operator*() const
		{
			return mPayload.template Ref<D>();
		}


		D* operator->()
		{
			return mPayload.template Ptr<D>().Unwrap();
		}


		const D* operator->() const
		{
			return mPayload.template Ptr<D>().Unwrap();
		}


		const D& Value() const
		{
			Assert(IsOk());
			return mPayload.template Ref<D>();
		}


		D Unwrap()
		{
			return mPayload.template Take<D>().Unwrap();
		}


		D UnwrapOr(auto&& value)
		{
			return IsOk() ? Unwrap() : D(value);
		}


		const E& Err()
		{
			return mPayload.template Ref<E>();
		}


		explicit operator bool() const
		{
			return IsOk();
		}


		Optional<D> IntoOptional()
		{
			if (IsOk())
			{
				return this->Unwrap();
			}
			else
			{
				return NullOpt;
			}
		}


		template<std::invocable<D&&> F>
		Result<std::invoke_result_t<F, D&&>, E> Map(F&& f)
		{
			if (*this)
			{
				return std::invoke(std::forward<F>(f), mPayload.template Take<D>().Unwrap());
			}
			else
			{
				return Err();
			}
		}


		bool operator<(auto&& value) const
		{
			return mPayload < value;
		}

		bool operator>(auto&& value) const
		{
			return mPayload > value;
		}

		bool operator<=(auto&& value) const
		{
			return mPayload <= value;
		}

		bool operator>=(auto&& value) const
		{
			return mPayload >= value;
		}

		bool operator==(auto&& value) const
		{
			return mPayload == value;
		}

		bool operator!=(auto&& value) const
		{
			return mPayload != value;
		}


	private:
		using Payload = Variant<D, E>;


		Result(const Payload& payload)
			: mPayload(payload) {}


		Result(Payload&& payload)
			: mPayload(std::forward<Payload>(payload)) {}


		Payload mPayload;
	};


	template<typename E>
	class [[nodiscard]] Result<void, E>
	{
	public:
		Result(auto&& value)
			: mPayload([]<typename T>(T&& value)
			{
				if constexpr (std::same_as<Success_t, std::decay_t<T>>)
				{
					return std::monostate();
				}
				else if constexpr (std::constructible_from<E, T>)
				{
					return E(std::forward<T>(value));
				}
				else
				{
					static_assert(false, "Cannot construct Result<void, E> with this argument!");
				}
			}(std::forward<decltype(value)>(value)))
		{}


		void Unwrap() noexcept
		{
			Assert(IsOk());
		}


		[[nodiscard]] bool IsOk() const
		{
			return mPayload.template IsType<std::monostate>();
		}


		[[nodiscard]] bool IsErr() const
		{
			return mPayload.template IsType<E>();
		}


		const E& Err()
		{
			return mPayload.template Ref<E>();
		}


		explicit operator bool()
		{
			return IsOk();
		}


		bool operator<(auto&& value) const
		{
			if constexpr (std::same_as<std::monostate, std::decay_t<decltype(value)>>)
			{
				return true;
			}
			else
			{
				return mPayload < value;
			}
		}

		bool operator>(auto&& value) const
		{
			if constexpr (std::same_as<std::monostate, std::decay_t<decltype(value)>>)
			{
				return false;
			}
			else
			{
				return mPayload > value;
			}
		}

		bool operator<=(auto&& value) const
		{
			if constexpr (std::same_as<std::monostate, std::decay_t<decltype(value)>>)
			{
				return true;
			}
			else
			{
				return mPayload <= value;
			}
		}

		bool operator>=(auto&& value) const
		{
			if constexpr (std::same_as<std::monostate, std::decay_t<decltype(value)>>)
			{
				return false;
			}
			else
			{
				return mPayload >= value;
			}
		}

		bool operator==(auto&& value) const
		{
			if constexpr (std::same_as<std::monostate, std::decay_t<decltype(value)>>)
			{
				return false;
			}
			else
			{
				return mPayload == value;
			}
		}

		bool operator!=(auto&& value) const
		{
			if constexpr (std::same_as<std::monostate, std::decay_t<decltype(value)>>)
			{
				return true;
			}
			else
			{
				return mPayload != value;
			}
		}


	private:
		using Payload = Core::Variant<std::monostate, E>;


		Result(const Payload& payload)
			: mPayload(payload) {}


		Result(Payload&& payload)
			: mPayload(std::forward<Payload>(payload)) {}


		Payload mPayload;
	};
} // namespace Strawberry::Core

#pragma once


#include "Strawberry/Core/Assert.hpp"
#include "Optional.hpp"
#include <variant>

#include "Strawberry/Core/Types/Variant.hpp"


namespace Strawberry::Core
{
	template<typename D, typename E> requires (!std::same_as<D, E>)
	class [[nodiscard]] Result
	{
	public:
		Result(const D& value) requires (std::copy_constructible<D>)
			: mPayload(value) {}


		Result(D&& value) requires (std::move_constructible<D>)
			: mPayload(std::move(value)) {}


		Result(const E& value) requires (std::copy_constructible<E>)
			: mPayload(value) {}


		Result(E&& value) requires (std::move_constructible<E>)
			: mPayload(std::move(value)) {}


		static Result Ok(const D& value) requires (std::copy_constructible<D>)
		{
			return Result(value);
		}


		static Result Ok(D&& value) requires (std::move_constructible<D>)
		{
			return Result(std::forward<D>(value));
		}


		template<typename... Args>
		static Result Ok(Args... args) requires (std::constructible_from<D, Args...>)
		{
			return Result(D(std::forward<Args>(args)...));
		}


		static Result Err(const E& value) requires (std::copy_constructible<E>)
		{
			return Result(value);
		}


		static Result Err(E&& value) requires (std::move_constructible<E>)
		{
			return Result(std::forward<E>(value));
		}


		template<typename... Args>
		static Result Err(Args... args) requires (std::constructible_from<E, Args...>)
		{
			return Result(E(std::forward<Args>(args)...));
		}


		[[nodiscard]] inline bool IsOk() const
		{
			return mPayload.template IsType<D>();
		}


		[[nodiscard]] inline bool IsErr() const
		{
			return mPayload.template IsType<E>();
		}


		D& operator*()
		{
			return mPayload.template Ref<D>();
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


		D Unwrap()
		{
			return mPayload.template Take<D>().Unwrap();
		}


		D UnwrapOr(D value)
		{
			return IsOk() ? Unwrap() : value;
		}


		const E& Err()
		{
			return mPayload.template Ref<E>();
		}


		explicit inline operator bool() const
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
		Result<std::invoke_result_t<F, D&&>, E> Map(F f)
		{
			if (*this)
			{
				return Result<std::invoke_result_t<F, D&&>, E>::Ok(f(std::move(Unwrap())));
			}
			else
			{
				return Result<std::invoke_result_t<F, D&&>, E>::Err(std::move(Err()));
			}
		}


		bool operator==(const D& other) const
		{
			return mPayload == other;
		}


		bool operator!=(const D& other) const
		{
			return mPayload != other;
		}


		bool operator==(const E& other) const
		{
			return mPayload == other;
		}


		bool operator!=(const E& other) const
		{
			return mPayload != other;
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
		Result()
			: mPayload() {}


		Result(const E& value) requires (std::copy_constructible<E>)
			: mPayload(value) {}


		Result(E&& value) requires (std::move_constructible<E>)
			: mPayload(std::move(value)) {}


		void Unwrap()
		{
			Assert(IsOk());
		}


		static Result Err(const E& value) requires (std::copy_constructible<E>)
		{
			return Result(false, value);
		}


		static Result Err(E&& value) requires (std::move_constructible<E>)
		{
			return Result(false, std::forward<E>(value));
		}


		template<typename... Args>
		static Result Err(Args... args) requires (std::constructible_from<E, Args...>)
		{
			return Result(false, E(std::forward<Args>(args)...));
		}


		[[nodiscard]] inline bool IsOk() const
		{
			return !mPayload;
		}


		[[nodiscard]] inline bool IsErr() const
		{
			return mPayload;
		}


		const E& Err()
		{
			return mPayload.Ref();
		}


		explicit inline operator bool()
		{
			return IsOk();
		}


		bool operator==(const E& other) const
		{
			return mPayload == other;
		}


		bool operator!=(const E& other) const
		{
			return mPayload != other;
		}

	private:
		using Payload = Core::Optional<E>;


		Result(const Payload& payload)
			: mPayload(payload) {}


		Result(Payload&& payload)
			: mPayload(std::forward<Payload>(payload)) {}


		Payload mPayload;
	};
} // namespace Strawberry::Core

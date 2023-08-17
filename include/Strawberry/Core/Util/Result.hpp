#pragma once


#include "Assert.hpp"
#include <variant>


namespace Strawberry::Core
{
	template <typename D, typename E>
		requires (!std::same_as<D, E>)
	class [[nodiscard]] Result
	{
	public:
		Result(const D& value)
			requires (std::copy_constructible<D>)
			: mIsOk(true)
			, mPayload(value)
		{}


		Result(D&& value)
			requires (std::move_constructible<D>)
			: mIsOk(true)
			, mPayload(std::move(value))
		{}


		Result(const E& value)
			requires (std::copy_constructible<E>)
			: mIsOk(false)
			, mPayload(value)
		{}


		Result(E&& value)
			requires (std::move_constructible<E>)
			: mIsOk(false)
			, mPayload(std::move(value))
		{}


		static Result Ok(const D& value)
			requires (std::copy_constructible<D>)
		{
			return Result(true, value);
		}


		static Result Ok(D&& value)
			requires (std::move_constructible<D>)
		{
			return Result(true, std::forward<D>(value));
		}


		template <typename... Args>
		static Result Ok(Args... args)
			requires (std::constructible_from<D, Args...>)
		{
			return Result(true, D(std::forward<Args>(args)...));
		}


		static Result Err(const E& value)
			requires (std::copy_constructible<E>)
		{
			return Result(false, value);
		}


		static Result Err(E&& value)
			requires (std::move_constructible<E>)
		{
			return Result(false, std::forward<E>(value));
		}


		template <typename... Args>
		static Result Err(Args... args)
			requires (std::constructible_from<E, Args...>)
		{
			return Result(false, E(std::forward<Args>(args)...));
		}


		[[nodiscard]] inline bool IsOk() const { return mIsOk; }


		[[nodiscard]] inline bool IsErr() const { return !mIsOk; }


		D& operator*()
		{
			Assert(IsOk());
			return std::get<D>(mPayload);
		}


		const D& operator*() const
		{
			Assert(IsOk());
			return std::get<D>(mPayload);
		}


		D* operator->()
		{
			Assert(IsOk());
			return &std::get<D>(mPayload);
		}


		const D* operator->() const
		{
			Assert(IsOk());
			return &std::get<D>(mPayload);
		}


		D Unwrap()
		{
			Assert(IsOk());
			return std::move(std::get<D>(mPayload));
		}


		D UnwrapOr(D value) { return IsOk() ? Unwrap() : value; }


		const E& Err()
		{
			Assert(IsErr());
			return std::move(std::get<E>(mPayload));
		}


		explicit inline operator bool() { return IsOk(); }


		template <std::invocable<D&&> F>
		Result<std::invoke_result_t<F, D&&>, E> Map(F f)
		{
			if (*this) { return Result<std::invoke_result_t<F, D&&>, E>::Ok(f(std::move(Unwrap()))); }
			else { return Result<std::invoke_result_t<F, D&&>, E>::Err(std::move(Err())); }
		}


	private:
		using Payload = std::variant<D, E>;


		Result(bool isOk, const Payload& payload)
			: mIsOk(isOk)
			, mPayload(payload)
		{}


		Result(bool isOk, Payload&& payload)
			: mIsOk(isOk)
			, mPayload(std::forward<Payload>(payload))
		{}


		bool    mIsOk;
		Payload mPayload;
	};
} // namespace Strawberry::Core
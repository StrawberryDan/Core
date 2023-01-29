#pragma once



#include <variant>
#include "Concepts.hpp"
#include "Core/Assert.hpp"



namespace Strawberry::Core
{
	template<typename D, typename E> requires ( !std::same_as<D, E> )
	class Result
	{
	public:
		Result(D value) requires ( std::is_trivially_copyable_v<D> )
			: mIsOk(true)
			, mPayload(value)
		{}

		Result(const D& value) requires ( std::copy_constructible<D> && !std::is_trivially_copyable_v<D> )
			: mIsOk(true)
			, mPayload(value)
		{}

		Result(D&& value) requires ( std::move_constructible<D> && !std::is_trivially_copyable_v<D> )
			: mIsOk(true)
			, mPayload(std::forward<D>(value))
		{}

		Result(E value) requires ( std::is_trivially_copyable_v<E> )
				: mIsOk(false)
				, mPayload(value)
		{}

		Result(const E& value) requires ( std::copy_constructible<E> && !std::is_trivially_copyable_v<E> )
		: mIsOk(false)
		, mPayload(value)
		{}

		Result(E&& value) requires ( std::move_constructible<E> && !std::is_trivially_copyable_v<E> )
		: mIsOk(false)
		, mPayload(std::forward<D>(value))
		{}

	    static Result Ok(const D& value) requires ( std::copy_constructible<D> )
	        { return Result(true, value); }

	    static Result Ok(D&& value) requires ( std::move_constructible<D> )
	        { return Result(true, std::forward<D>(value)); }

	    template <typename ...Args>
	    static Result   Ok(Args ...args) requires ( std::constructible_from<D, Args...> )
	        { return Result(true,  D(std::forward<Args>(args)...)); }



	    static Result Err(const E& value) requires ( std::copy_constructible<E> )
	        { return Result(false, value); }

	    static Result Err(E&& value) requires ( std::move_constructible<E> )
	        { return Result(false, std::forward<E>(value)); }

	    template <typename ...Args>
	    static Result  Err(Args ...args) requires ( std::constructible_from<E, Args...> )
	        { return Result(false, E(std::forward<Args>(args)...)); }



	    inline bool  IsOk() const { return  mIsOk; }
	    inline bool IsErr() const { return !mIsOk; }



	          D& operator *()        { Assert(IsOk()); return  std::get<D>(mPayload); }
	    const D& operator *()  const { Assert(IsOk()); return  std::get<D>(mPayload); }
	          D* operator->()       { Assert(IsOk()); return &std::get<D>(mPayload); }
	    const D* operator->() const { Assert(IsOk()); return &std::get<D>(mPayload); }



	    D          Unwrap()        { Assert(IsOk());  return std::move(std::get<D>(mPayload)); }
	    D        UnwrapOr(D value) { return IsOk() ? Unwrap() : value; }
	    const E&      Err()        { Assert(IsErr()); return std::move(std::get<E>(mPayload)); }



	    explicit inline operator bool() { return IsOk(); }



	    template<typename U, Callable<U, D&&> F>
	    Result<U, E> Map(F f)
	    {
	        if (*this)
	        {
	            return Result<U, E>::Ok(f(std::move(Unwrap())));
	        }
	        else
	        {
	            return Result<U, E>::Err(std::move(Err()));
	        }
	    }

	private:
	    using Payload = std::variant<D, E>;

		Result(bool isOk, const Payload&  payload) : mIsOk(isOk), mPayload(payload) {}
	    Result(bool isOk,       Payload&& payload) : mIsOk(isOk), mPayload(std::forward<Payload>(payload)) {}

	    bool mIsOk;
	    Payload mPayload;
	};
}
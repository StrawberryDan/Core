#pragma once



#include <cstdint>
#include <utility>
#include <concepts>


#include "Assert.hpp"
#include "Concepts.hpp"
#include "Utilities.hpp"



namespace Strawberry::Core
{
	class NullOpt_t
	{
	public:
		explicit NullOpt_t(int) {}
	};


	static const NullOpt_t NullOpt = NullOpt_t{0};


	template<typename T>
	class Option
	{
	public:
		Option()
			: mHasValue(false)
		{}



		Option(NullOpt_t)
			: mHasValue(false)
		{}



		Option(const T& value) requires (std::is_copy_constructible_v<T>)
			: mHasValue(true), mPayload(value)
		{}



		Option(T&& value) requires (std::is_move_constructible_v<T>)
			: mHasValue(true), mPayload(std::move(value))
		{}



		template <typename... Ts>
		explicit Option(Ts... ts) requires ( std::constructible_from<T, Ts...> )
			: mHasValue(true)
			, mPayload(std::forward<Ts>(ts)...)
		{}



		Option(const Option& rhs) requires (std::is_copy_constructible_v<T>)
			: mHasValue(rhs.mHasValue)
		{
			if (rhs)
			{
				std::construct_at(&mPayload, *rhs);
			}
		}



		Option(Option&& rhs) requires(std::is_move_constructible_v<T>)
			: mHasValue(rhs.mHasValue)
		{
			if (rhs)
			{
				std::construct_at(&mPayload, std::move(*rhs));
				rhs.mHasValue = false;
			}
		}



		Option& operator=(NullOpt_t)
		{
			Reset();
		}



		Option& operator=(const T& rhs) requires(std::is_copy_assignable_v<T>)
		{
			if (mHasValue)
			{
				std::destroy_at(&mPayload);
			}

			mHasValue = true;
			std::construct_at(&mPayload, rhs);

			return *this;
		}



		Option& operator=(const Option& rhs) requires(std::is_copy_assignable_v<T>)
		{
			if (this != &rhs)
			{
				if (!rhs.mHasValue && mHasValue)
				{
					std::destroy_at(&mPayload);
				}

				if (rhs.mHasValue && mHasValue)
				{
					mPayload = *rhs;
				}
				else if (rhs.mHasValue && !mHasValue)
				{
					std::construct_at(&mPayload, *rhs);
				}

				mHasValue = rhs.mHasValue;
			}

			return *this;
		}



		Option& operator=(T&& rhs) requires(std::is_move_assignable_v<T>)
		{
			if (mHasValue)
			{
				std::destroy_at(&mPayload);
			}

			mHasValue = true;
			std::construct_at(&mPayload, std::move(rhs));

			return *this;
		}



		Option& operator=(Option&& rhs) requires(std::is_move_assignable_v<T>)
		{
			if (this != &rhs)
			{
				if (!rhs.mHasValue && mHasValue)
				{
					Reset();
				}

				if (rhs.mHasValue && mHasValue)
				{
					mPayload = std::move(rhs.Unwrap());
				}
				else if (rhs.mHasValue && !mHasValue)
				{
					Emplace(std::move(rhs.Unwrap()));
				}
			}

			return *this;
		}



		~Option()
		{
			if (mHasValue)
			{
				mHasValue = false;
				std::destroy_at(&mPayload);
			}
		}



		template<typename ...Args>
		void Emplace(Args ...args)
		{
			if (mHasValue)
			{
				std::destroy_at(&mPayload);
			}

			mHasValue = true;
			std::construct_at(&mPayload, std::forward<Args>(args)...);
		}



		void Reset()
		{
			if (mHasValue)
			{
				std::destroy_at(&mPayload);
				mHasValue = false;
			}
		}



		inline bool HasValue() const
		{ return mHasValue; }



		explicit inline operator bool() const
		{ return mHasValue; }



		const T& Value() const
		{
			Assert(HasValue());
			return mPayload;
		}


		const T& ValueOr(const T& value) const
		{
			if (HasValue())
			{
				return Value();
			}
			else
			{
				return value;
			}
		}


		T& operator*()
		{
			Assert(mHasValue);
			return mPayload;
		}



		const T& operator*() const
		{
			Assert(mHasValue);
			return mPayload;
		}



		T* operator->()
		{
			Assert(mHasValue);
			return &mPayload;
		}



		const T* operator->() const
		{
			Assert(mHasValue);
			return &mPayload;
		}



		T Unwrap()
		{
			Assert(mHasValue);
			mHasValue = false;
			return std::move(mPayload);
		}



		T UnwrapOr(T&& value)
		{
			if (HasValue())
			{
				return Unwrap();
			}
			else
			{
				return value;
			}
		}



		template<std::invocable<const T&> F>
		Option<std::invoke_result_t<F, const T&>> Map(F functor) const&
		{
			if (HasValue())
			{
				return Option<std::invoke_result_t<F, const T&>>(functor(Value()));
			}
			else
			{
				return {};
			}
		}



		template<std::invocable<T&&> F>
		Option<std::invoke_result_t<F, T&&>> Map(F functor) &&
		{
			if (HasValue())
			{
				return Option<std::invoke_result_t<F, T&&>>(functor(std::move(Unwrap())));
			}
			else
			{
				return {};
			}
		}



		Option<T*> AsPtr()
		{
			if (HasValue())
			{
				return &mPayload;
			}
			else
			{
				return {};
			}
		}



		Option<const T*> AsPtr() const
		{
			if (HasValue())
			{
				return &mPayload;
			}
			else
			{
				return {};
			}
		}



		template <std::equality_comparable_with<T> R>
		bool operator==(const Option<R>& rhs)
		{
			if (!HasValue() && !rhs.HasValue())
			{
				return true;
			}
			else if (HasValue() && rhs.HasValue())
			{
				return (**this) == (*rhs);
			}

			return false;
		}



		template <std::equality_comparable_with<T> R>
		inline bool operator!=(const Option<R>& rhs) const
		{
			if (!HasValue() && !rhs.HasValue())
			{
				return false;
			}
			else if (HasValue() && rhs.HasValue())
			{
				return (**this) != (*rhs);
			}

			return true;
		}



		template <std::equality_comparable_with<T> R>
		inline bool operator==(const R& rhs)
		{
			if (!HasValue())
			{
				return false;
			}
			else
			{
				return (**this) == rhs;
			}
		}



		template <std::equality_comparable_with<T> R>
		inline bool operator!=(const R& rhs)
		{
			if (!HasValue())
			{
				return true;
			}
			else
			{
				return (**this) != rhs;
			}
		}



	private:
		bool    mHasValue;
		union
		{
			T mPayload;
		};
	};



	template<typename T> requires ( std::is_pointer_v<T> )
	class Option<T>
	{
	public:
		Option()
			: mPayload(nullptr)
		{}



		Option(NullOpt_t)
			: mPayload(nullptr)
		{}



		Option(T value)
			: mPayload(value)
		{}



		Option(const Option& rhs)
			: mPayload(rhs.HasValue() ? *rhs : nullptr)
		{}



		Option(Option&& rhs)
			: mPayload(std::exchange(rhs.mPayload, nullptr))
		{}



		Option& operator=(NullOpt_t)
		{
			Reset();
		}



		Option& operator=(T rhs)
		{
			mPayload = rhs;

			return *this;
		}



		Option& operator=(const Option& rhs) requires(std::is_copy_assignable_v<T>)
		{
			if (this != &rhs)
			{
				mPayload = rhs.mPayload;
			}

			return *this;
		}



		Option& operator=(Option&& rhs) requires(std::is_move_assignable_v<T>)
		{
			if (this != &rhs)
			{
				mPayload = std::exchange(rhs.mPayload, nullptr);
			}

			return *this;
		}



		void Reset()
		{
			mPayload = nullptr;
		}



		inline bool HasValue() const
		{ return mPayload != nullptr; }



		explicit inline operator bool() const
		{ return HasValue(); }


		T Value() const
		{
			Assert(HasValue());
			return mPayload;
		}


		T ValueOr(T value) const
		{
			if (HasValue())
			{
				return Value();
			}
			else
			{
				return value;
			}
		}



		T operator*()
		{
			Assert(HasValue());
			return mPayload;
		}



		const T operator*() const
		{
			Assert(HasValue());
			return mPayload;
		}



		T operator->()
		{
			Assert(HasValue());
			return mPayload;
		}



		const T operator->() const
		{
			Assert(HasValue());
			return mPayload;
		}



		T Unwrap()
		{
			Assert(HasValue());
			return std::exchange(mPayload, nullptr);
		}



		T UnwrapOr(T&& value)
		{
			if (HasValue())
			{
				return Unwrap();
			}
			else
			{
				return value;
			}
		}



		template<std::invocable<const T> F>
		Option<std::invoke_result_t<F, const T>> Map(F functor) const&
		{
			if (HasValue())
			{
				return Option<std::invoke_result_t<F, const T&>>(functor(Value()));
			}
			else
			{
				return {};
			}
		}



		template<std::invocable<T> F>
		Option<std::invoke_result_t<F, T>> Map(F functor) &&
		{
			if (HasValue())
			{
				return Option<std::invoke_result_t<F, T&&>>(functor(std::move(Unwrap())));
			}
			else
			{
				return {};
			}
		}



		Option<T*> AsPtr()
		{
			if (HasValue())
			{
				return &mPayload;
			}
			else
			{
				return {};
			}
		}



		Option<const T*> AsPtr() const
		{
			if (HasValue())
			{
				return &mPayload;
			}
			else
			{
				return {};
			}
		}







		template <std::equality_comparable_with<T> R>
		bool operator==(const Option<R>& rhs)
		{
			if (!HasValue() && !rhs.HasValue())
			{
				return true;
			}
			else if (HasValue() && rhs.HasValue())
			{
				return (**this) == (*rhs);
			}

			return false;
		}



		template <std::equality_comparable_with<T> R>
		inline bool operator!=(const Option<R>& rhs) const
		{
			if (!HasValue() && !rhs.HasValue())
			{
				return false;
			}
			else if (HasValue() && rhs.HasValue())
			{
				return (**this) != (*rhs);
			}

			return true;
		}



		template <std::equality_comparable_with<T> R>
		inline bool operator==(const R& rhs)
		{
			if (!HasValue())
			{
				return false;
			}
			else
			{
				return (**this) == rhs;
			}
		}



		template <std::equality_comparable_with<T> R>
		inline bool operator!=(const R& rhs)
		{
			if (!HasValue())
			{
				return true;
			}
			else
			{
				return (**this) != rhs;
			}
		}



	private:
		T mPayload;
	};



	template <typename T>
	inline bool operator==(const T& a, const Option<T>& b) requires ( std::equality_comparable<T> )
	{
		return b == a;
	}



	template <typename T>
	inline bool operator!=(const T& a, const Option<T>& b) requires ( std::equality_comparable<T> )
	{
		return b != a;
	}
}
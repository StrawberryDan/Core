#pragma once



#include <cstdint>
#include <utility>
#include <concepts>


#include "Assert.hpp"
#include "Concepts.hpp"
#include "Utilities.hpp"



namespace Strawberry::Core
{
	template<typename T>
	class Option
	{
	public:
		Option()
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
					std::destroy_at(&mPayload);
				}

				if (rhs.mHasValue && mHasValue)
				{
					mPayload = std::move(rhs.Take());
				}
				else if (rhs.mHasValue && !mHasValue)
				{
					std::construct_at(&mPayload, std::move(rhs.Take()));
				}

				mHasValue = rhs.mHasValue;
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



		template<typename R, Callable<R, T&&> F>
		Option<R> Map(F functor)
		{
			if (HasValue())
			{
				return Option<R>(functor(std::move(Unwrap())));
			}
			else
			{
				return {};
			}
		}



		template<typename R, typename F>
		R MapOr(F functor, R&& value)
		requires Callable<R, F, T>
		{
			if (HasValue())
			{
				return Option<R>(F(Unwrap()));
			}
			else
			{
				return value;
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



		T&& Take()
		{
			mHasValue = false;
			return std::move(mPayload);
		}



		bool operator==(const Option<T>& rhs) const requires ( std::equality_comparable<T> )
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



		inline bool operator!=(const Option<T>& rhs) const requires ( std::equality_comparable<T> )
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



		inline bool operator==(const T& rhs) const requires ( std::equality_comparable<T> )
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



		inline bool operator!=(const T& rhs) const requires ( std::equality_comparable<T> )
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
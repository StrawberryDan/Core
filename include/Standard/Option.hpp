#pragma once



#include <cstdint>
#include <utility>
#include <concepts>


#include "Standard/Assert.hpp"
#include "Standard/Concepts.hpp"
#include "Standard/Utilities.hpp"



namespace Strawberry::Standard
{
	template<typename T>
	class Option
	{
	public:
		Option()
			: mHasValue(false)
			, mData{}
		{}



		template<typename ...Args>
		Option(Args ...args) requires ( std::is_constructible_v<T, Args...> )
		: mHasValue(true)
		, mData{}
		{
			new (mData) T(std::forward<Args>(args)...);
		}



		Option(const Option& rhs) requires ( std::is_copy_constructible_v<T> )
		: mHasValue(rhs.mHasValue)
		, mData{}
		{
			if (rhs)
			{
				new(mData) T(*rhs);
			}
		}



		Option(Option&& rhs)  noexcept requires ( std::is_move_constructible_v<T> )
		: mHasValue(false)
		, mData{}
		{
			if (rhs)
			{
				new (mData) T(std::move(*rhs));
				mHasValue = Replace(rhs.mHasValue, false);
				std::fill(rhs.mData, rhs.mData + sizeof(T), 0);
			}
		}



		Option& operator=(const Option& rhs) requires ( std::is_copy_assignable_v<T> )
		{
			if (this != &rhs)
			{
				if (mHasValue)
				{
					(*this)->~T();
				}

				mHasValue = rhs.mHasValue;
				if (mHasValue)
				{
					new(mData) T(*rhs);
				}
			}

			return *this;
		}



		Option& operator=(Option&& rhs) noexcept requires ( std::is_move_assignable_v<T> )
		{
			if (this != &rhs)
			{
				if (mHasValue)
				{
					(*this)->~T();
					std::fill(mData, mData + sizeof(T), 0);
				}

				mHasValue = rhs.mHasValue;
				if (mHasValue)
				{
					new (mData) T(std::move(*rhs));
					rhs.mHasValue = false;
					std::fill(rhs.mData, rhs.mData + sizeof(T), 0);
				}
			}

			return *this;
		}



		~Option()
		{
			if (mHasValue)
			{
				reinterpret_cast<T*>(mData)->~T();
			}
		}


		template<typename ...Args>
		void Emplace(Args ...args)
		{
			if (mHasValue)
			{
				(*this)->~T();
			}

			new (mData) T(std::forward<Args>(args)...);
			mHasValue = true;
		}

		void Reset()
		{
			if (mHasValue)
			{
				reinterpret_cast<T*>(mData)->~T();
				mHasValue = false;
			}
		}


		inline bool HasValue() const { return mHasValue; }
		explicit inline operator bool() const { return mHasValue; }


			  T& operator *()       { Assert(mHasValue); return *reinterpret_cast<      T*>(mData); }
		const T& operator *() const { Assert(mHasValue); return *reinterpret_cast<const T*>(mData); }

			  T* operator->()       requires (!std::is_pointer_v<T>) { Assert(mHasValue); return  reinterpret_cast<      T*>(mData); }
		const T* operator->() const requires (!std::is_pointer_v<T>) { Assert(mHasValue); return  reinterpret_cast<const T*>(mData); }
		      T  operator->()       requires ( std::is_pointer_v<T>) { Assert(mHasValue); return  reinterpret_cast<      T >(mData); }
		const T  operator->() const requires ( std::is_pointer_v<T>) { Assert(mHasValue); return  reinterpret_cast<const T >(mData); }



		T Unwrap()
		{
			Assert(mHasValue);
			return std::move(**this);
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




		template <typename R, typename F>
		R Map(F functor) requires Callable<R, F, T>
		{
			if (HasValue())
			{
				return Option<R>(F(Unwrap()));
			}
			else
			{
				return {};
			}
		}



		template <typename R, typename F>
		R MapOr(F functor, R&& value) requires Callable<R, F, T>
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
		uint8_t mData[sizeof(T)];
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
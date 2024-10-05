#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Strawberry Core
#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/Concepts.hpp"
// Standard Library
#include <concepts>


namespace Strawberry::Core
{
	//======================================================================================================================
	//  Pre-declare Optional
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	class Optional;


	//======================================================================================================================
	//  Type Deduction Guide
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	Optional(T) -> Optional<std::decay_t<T>>;


	//======================================================================================================================
	//  IsOptional Type Trait
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	struct IsOptional_v
			: std::false_type {};


	template<typename T>
	struct IsOptional_v<Optional<T>>
			: std::true_type {};


	template<typename T>
	concept IsOptional = IsOptional_v<T>::value;


	//======================================================================================================================
	//  NullOpt type
	//----------------------------------------------------------------------------------------------------------------------
	class NullOpt_t
	{
	public:
		explicit NullOpt_t(int) {}
	};


	static const NullOpt_t NullOpt = NullOpt_t{0};


	template <typename T>
	class OptionalCommon
	{
	public:
		auto& operator=(this auto& self, NullOpt_t) noexcept
		{
			self.Reset();
			return self;
		}


		T UnwrapOr(this auto self, std::convertible_to<T> auto value)
		{
			if (self.HasValue()) return self.Unwrap();
			else return static_cast<T>(std::forward<decltype(value)>(value));
		}


		const T& ValueOr(this auto const& self, const T& value)
		{
			if (self.HasValue())
			{
				return self.Value();
			}
			else
			{
				return value;
			}
		}


		explicit operator bool(this auto const& self)
		{
			return self.HasValue();
		}


		//======================================================================================================================
		//  Monadic Operations
		//----------------------------------------------------------------------------------------------------------------------
		template<std::invocable<const T&> F>
		Optional<std::invoke_result_t<F, const T&>>
		Map(this auto const& self, F&& functor)
		{
			return self.HasValue() ? Optional<std::invoke_result_t<F, const T&>>(std::forward<F>(functor)(self.Value())) : NullOpt;
		}


		template<std::invocable<T&&> F>
		Optional<std::invoke_result_t<F, T&&>>
		Map(this auto&& self, F&& functor)
		{
			return self.HasValue() ? Optional<std::invoke_result_t<F, T&&>>(std::forward<F>(functor)(self.Unwrap())) : NullOpt;
		}


		template<std::invocable<const T&> F> requires IsOptional<std::invoke_result_t<F, const T&>>
		std::invoke_result_t<F, const T&>
		AndThen(this auto const& self, F&& functor)
		{
			return self.Map(std::forward<F>(functor)).Flatten();
		}


		template<std::invocable<T&&> F> requires IsOptional<std::invoke_result_t<F, T&&>>
		std::invoke_result_t<F, T&&>
		AndThen(this auto&& self, F&& functor)
		{
			return self.Map(std::forward<F>(functor)).Flatten();
		}


		auto Flatten(this auto const& self) requires (IsOptional<T>)
		{
			return self.HasValue() ? self.Value() : NullOpt;
		}


		auto Flatten(this auto&& self) requires (IsOptional<T>)
		{
			return self.HasValue() ? self.Unwrap() : NullOpt;
		}


		auto AsPtr(this auto& self)
		{
			return self.HasValue() ? Optional(&self.Value()) : NullOpt;
		}


		auto Deref(this auto& self) requires (Dereferencable<T>)
		{
			return self.HasValue() ? Optional(*self.Value()) : NullOpt;
		}


		auto& Ref(this auto& self)
		{
			Assert(self.HasValue());
			return self.Value();
		}


		//======================================================================================================================
		//  Comparison Operators
		//----------------------------------------------------------------------------------------------------------------------
		template<std::equality_comparable_with<T> R>
		bool operator==(this auto const& self, const Optional<R>& rhs)
		{
			if (!self.asValue() && !rhs.HasValue())
			{
				return true;
			}
			else if (self.HasValue() && rhs.HasValue())
			{
				return self.Value() == rhs.Value();
			}

			return false;
		}


		template<std::equality_comparable_with<T> R>
		inline bool operator!=(this auto const& self, const Optional<R>& rhs)
		{
			if (!self.HasValue() && !rhs.HasValue())
			{
				return false;
			}
			else if (self.HasValue() && rhs.HasValue())
			{
				return self.Value() != rhs.Value();
			}

			return true;
		}


		template<std::totally_ordered_with<T> R>
		inline bool operator>(this auto const& self, const Optional<R> rhs)
		{
			if (self.HasValue() && rhs.HasValue())
			{
				return self.Value() > rhs.Value();
			}
			else if (!self.HasValue() && rhs.HasValue())
			{
				return false;
			}
			else if (self.HasValue() && !rhs.HasValue())
			{
				return true;
			}
			else if (!self.HasValue() && !rhs.HasValue())
			{
				return false;
			}
		}


		template<std::totally_ordered_with<T> R>
		inline bool operator>=(this auto const& self, const Optional<R> rhs)
		{
			if (self.HasValue() && rhs.HasValue())
			{
				return self.Value() >= rhs.Value();
			}
			else if (!self.HasValue() && rhs.HasValue())
			{
				return false;
			}
			else if (self.HasValue() && !rhs.HasValue())
			{
				return true;
			}
			else if (!self.HasValue() && !rhs.HasValue())
			{
				return true;
			}
		}


		template<std::totally_ordered_with<T> R>
		inline bool operator<(this auto const& self, const Optional<R> rhs)
		{
			if (self.HasValue() && rhs.HasValue())
			{
				return self.Value() < rhs.Value();
			}
			else if (!self.HasValue() && rhs.HasValue())
			{
				return true;
			}
			else if (self.HasValue() && !rhs.HasValue())
			{
				return false;
			}
			else if (!self.HasValue() && !rhs.HasValue())
			{
				return false;
			}
		}


		template<std::totally_ordered_with<T> R>
		inline bool operator<=(this auto const& self, const Optional<R> rhs)
		{
			if (self.HasValue() && rhs.HasValue())
			{
				return self.Value() <= rhs.Value();
			}
			else if (!self.HasValue() && rhs.HasValue())
			{
				return true;
			}
			else if (self.HasValue() && !rhs.HasValue())
			{
				return false;
			}
			else if (!self.HasValue() && !rhs.HasValue())
			{
				return true;
			}
		}


		template<std::equality_comparable_with<T> R>
		inline bool operator==(this auto const& self, const R& rhs)
		{
			if (!self.HasValue())
			{
				return false;
			}
			else
			{
				return self.Value() == rhs;
			}
		}


		template<std::equality_comparable_with<T> R>
		inline bool operator!=(this auto const& self, const R& rhs)
		{
			if (!self.HasValue())
			{
				return true;
			}
			else
			{
				return self.Value() != rhs;
			}
		}


		template<std::totally_ordered_with<T> R>
		inline bool operator>(this auto const& self, const R& rhs)
		{
			if (self.HasValue())
			{
				return self.Value() > rhs;
			}
			else
			{
				return false;
			}
		}


		template<std::totally_ordered_with<T> R>
		inline bool operator>=(this auto const& self, const R& rhs)
		{
			if (self.HasValue())
			{
				return self.Value() >= rhs;
			}
			else
			{
				return false;
			}
		}


		template<std::totally_ordered_with<T> R>
		inline bool operator<(this auto const& self, const R& rhs)
		{
			if (self.HasValue())
			{
				return self.Value() < rhs;
			}
			else
			{
				return true;
			}
		}


		template<std::totally_ordered_with<T> R>
		inline bool operator<=(this auto const& self, const R& rhs)
		{
			if (self.HasValue())
			{
				return self.Value() <= rhs;
			}
			else
			{
				return true;
			}
		}
	};


	//======================================================================================================================
	//  Base Optional Class
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	class [[nodiscard]] Optional
		: public OptionalCommon<T>
	{
	public:
		using Inner = T;

	public:
		//======================================================================================================================
		//  Construction, Destruction and Assignment
		//----------------------------------------------------------------------------------------------------------------------
		Optional()
			: mHasValue(false) {}


		Optional(NullOpt_t)
			: mHasValue(false) {}


		template<typename... Ts>
		Optional(Ts&&... ts) requires (std::constructible_from<T, Ts...>)
			: mHasValue(true)
			, mPayload(std::forward<Ts>(ts)...) {}


		Optional(const Optional& rhs) requires(std::is_copy_constructible_v<T>)
			: mHasValue(rhs.mHasValue)
		{
			if (rhs)
			{
				std::construct_at(&mPayload, rhs.Value());
			}
		}


		Optional(Optional&& rhs) noexcept requires(std::is_move_constructible_v<T>)
			: mHasValue(rhs.mHasValue)
		{
			if (rhs)
			{
				std::construct_at(&mPayload, std::forward<T>(rhs.Unwrap()));
			}
		}


		template<typename T2> requires (std::assignable_from<T, T2> && std::constructible_from<T, T2>)
		Optional& operator=(T2&& t2)
		{
			if (HasValue())
			{
				mPayload = std::forward<T2>(t2);
			}
			else
			{
				Emplace(std::forward<T2>(t2));
			}
		}


		Optional& operator=(const Optional& rhs) noexcept requires(std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>)
		{
			if (this != &rhs)
			{
				if (!rhs.HasValue() && HasValue()) Reset();
				else if (rhs.HasValue() && HasValue()) mPayload = *rhs;
				else if (rhs.HasValue() && !HasValue()) Emplace(*rhs);
			}

			return *this;
		}


		Optional& operator=(const Optional& rhs) noexcept requires(std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T>)
		{
			if (this != &rhs)
			{
				if (!rhs.HasValue()) Reset();
				else Emplace(*rhs);
			}

			return *this;
		}


		Optional& operator=(Optional&& rhs) noexcept requires(std::is_move_constructible_v<T> && std::is_move_assignable_v<T>)
		{
			if (this != &rhs)
			{
				if (!rhs.HasValue() && HasValue()) Reset();
				else if (rhs.HasValue() && HasValue()) mPayload = std::move(rhs.Unwrap());
				else if (rhs.HasValue() && !HasValue()) Emplace(std::move(rhs.Unwrap()));
			}

			return *this;
		}


		Optional& operator=(T&& rhs) noexcept requires(std::is_move_constructible_v<T> && !std::is_move_assignable_v<T>)
		{
			Emplace(std::move(rhs));

			return *this;
		}


		Optional& operator=(Optional&& rhs) noexcept requires(std::is_move_constructible_v<T> && !std::is_move_assignable_v<T>)
		{
			if (this != &rhs)
			{
				if (!rhs.HasValue()) Reset();
				else Emplace(std::move(rhs.Unwrap()));
			}

			return *this;
		}


		~Optional()
		{
			if (mHasValue)
			{
				mHasValue = false;
				std::destroy_at(&mPayload);
			}
		}


		//======================================================================================================================
		//  State Modification
		//----------------------------------------------------------------------------------------------------------------------
		template<typename... Args>
		void Emplace(Args&&... args)
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


		[[nodiscard]] inline bool HasValue() const
		{
			return mHasValue;
		}


		//======================================================================================================================
		//  Value Access
		//----------------------------------------------------------------------------------------------------------------------
		T& Value()
		{
			Assert(HasValue());
			return mPayload;
		}


		const T& Value() const
		{
			Assert(HasValue());
			return mPayload;
		}


		T& operator*()
		{
			Assert(HasValue());
			return mPayload;
		}


		const T& operator*() const
		{
			Assert(HasValue());
			return mPayload;
		}


		T* operator->()
		{
			Assert(HasValue());
			return &mPayload;
		}


		const T* operator->() const
		{
			Assert(HasValue());
			return &mPayload;
		}


		T Unwrap()
		{
			Assert(HasValue());
			mHasValue = false;
			return std::move(mPayload);
		}


	private:
		bool mHasValue;


		union
		{
			T mPayload;
		};
	};


	//======================================================================================================================
	//  Optional specialisation for pointers
	//----------------------------------------------------------------------------------------------------------------------
	template<typename T> requires(std::is_pointer_v<T>)
	class Optional<T>
		: public OptionalCommon<T>
	{
	public:
		using Inner = T;

	public:
		Optional()
			: mPayload(nullptr) {}


		Optional(NullOpt_t)
			: mPayload(nullptr) {}


		Optional(T value)
			: mPayload(value) {}


		Optional(const Optional& rhs) noexcept
			: mPayload(rhs.HasValue() ? *rhs : nullptr) {}


		Optional(Optional&& rhs) noexcept
			: mPayload(std::exchange(rhs.mPayload, nullptr)) {}


		Optional& operator=(T rhs) noexcept
		{
			mPayload = rhs;

			return *this;
		}


		Optional& operator=(const Optional& rhs) noexcept requires(std::is_copy_assignable_v<T>)
		{
			if (this != &rhs)
			{
				mPayload = rhs.mPayload;
			}

			return *this;
		}


		Optional& operator=(Optional&& rhs) noexcept requires(std::is_move_assignable_v<T>)
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


		[[nodiscard]] inline bool HasValue() const
		{
			return mPayload != nullptr;
		}


		T Value() const
		{
			Assert(HasValue());
			return mPayload;
		}


		T operator*()
		{
			Assert(HasValue());
			return mPayload;
		}


		T operator*() const
		{
			Assert(HasValue());
			return mPayload;
		}


		T operator->()
		{
			Assert(HasValue());
			return mPayload;
		}


		T operator->() const
		{
			Assert(HasValue());
			return mPayload;
		}


		T Unwrap()
		{
			Assert(HasValue());
			return std::exchange(mPayload, nullptr);
		}


		T UnwrapOr(std::nullptr_t)
		{
			if (HasValue()) return Unwrap();
			else return nullptr;
		}


	private:
		T mPayload;
	};


	template<typename T, typename R>
	inline bool operator==(const T& a, const Optional<R>& b) requires (std::equality_comparable_with<T, R>)
	{
		return b == a;
	}


	template<typename T, typename R>
	inline bool operator!=(const T& a, const Optional<R>& b) requires (std::equality_comparable_with<T, R>)
	{
		return b != a;
	}
} // namespace Strawberry::Core

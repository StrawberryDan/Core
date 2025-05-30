#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Core
#include "Strawberry/Core/Assert.hpp"
#include "Strawberry/Core/Types/Optional.hpp"
// Standard Library
#include <memory>
#include <mutex>

namespace Strawberry::Core
{
	template<typename T>
	class Mutex;


	template<typename T>
	class MutexGuard
	{
		template<typename>
		friend class Mutex;

	public:
		using LockType = std::unique_lock<std::recursive_mutex>;

		MutexGuard(const MutexGuard& other) = delete;

		MutexGuard(MutexGuard&& other) = default;

		MutexGuard& operator=(const MutexGuard&) = delete;

		MutexGuard& operator=(MutexGuard&& other) noexcept = default;


		const LockType& GetLock() const { return mLock; }
		      LockType& GetLock()       { return mLock; }


		inline T& operator*() const
		{
			return *mPayload;
		}


		inline T* operator->() const
		{
			return mPayload;
		}

	private:
		MutexGuard(LockType lock, T* ptr)
			: mLock(std::move(lock))
			, mPayload(ptr) {}


		LockType mLock;
		T*       mPayload;
	};


	template<typename T>
	class Mutex
	{
	public:
		template<typename... Ts>
		Mutex(Ts&&... ts) requires (std::constructible_from<T, Ts...>)
			: mMutex()
			, mPayload(std::forward<Ts>(ts)...) {}


		Mutex(Mutex&& rhs) noexcept requires (std::is_move_constructible_v<T>)
			: mMutex()
			, mPayload(std::move(rhs).Take()) {}


		Mutex& operator=(Mutex&& other) noexcept requires (std::is_move_assignable_v<T>)
		{
			if (this != &other)
			{
				std::destroy_at(this);
				*Lock() = std::move(other).Take();
			}

			return *this;
		}


		~Mutex()
		{
			ZoneScoped;

			auto lock = Lock();
			std::destroy_at(&*lock);
		}


		MutexGuard<T> Lock() &
		{
			ZoneScoped;

			return {std::unique_lock(mMutex), &mPayload};
		}


		MutexGuard<const T> Lock() const &
		{
			ZoneScoped;

			return {std::unique_lock(mMutex), &mPayload};
		}


		Optional<MutexGuard<T>> TryLock() &
		{
			ZoneScoped;

			std::unique_lock<std::recursive_mutex> lk(mMutex, std::defer_lock);
			if (lk.try_lock())
			{
				return MutexGuard<T>(std::move(lk), &mPayload);
			}
			else
			{
				return {};
			}
		}


		Optional<MutexGuard<const T>> TryLock() const &
		{
			ZoneScoped;

			std::unique_lock lk(mMutex, std::defer_lock);
			if (lk.try_lock())
			{
				return MutexGuard<T>(std::move(lk), &mPayload);
			}
			else
			{
				return {};
			}
		}


		Mutex<T> Clone() &
		{
			auto lock = Lock();
			return Mutex<T>(*lock);
		}


		T&& Take() &&
		{
			auto lock = Lock();
			return std::move(*lock);
		}

	private:
		mutable std::recursive_mutex mMutex;


		union
		{
			T mPayload;
		};
	};


	template<typename T>
	class SharedMutex
	{
	public:
		explicit SharedMutex(std::nullptr_t)
			: mPayload(nullptr) {}


		template<typename... Ts> requires (std::constructible_from<T, Ts...>)
		explicit SharedMutex(Ts&&... ts)
			: mPayload(std::make_shared<Mutex<T>>(std::forward<Ts>(ts)...)) {}


		SharedMutex& operator=(std::nullptr_t)
		{
			mPayload = nullptr;
			return *this;
		}


		bool HasValue() const
		{
			return mPayload.operator bool();
		}


		operator bool() const
		{
			return HasValue();
		}


		template<typename... Ts> requires std::constructible_from<T, Ts...>
		void Emplace(Ts&&... ts) &
		{
			mPayload = std::make_shared<Mutex<T>>(std::forward<Ts>(ts)...);
		}


		MutexGuard<T> Lock()
		{
			return mPayload->Lock();
		}


		MutexGuard<const T> Lock() const
		{
			return static_cast<const Mutex<T>*>(mPayload.get())->Lock();
		}


		Optional<MutexGuard<T>> TryLock()
		{
			return mPayload->TryLock();
		}


		Optional<MutexGuard<const T>> TryLock() const
		{
			return mPayload->TryLock();
		}

	private:
		std::shared_ptr<Mutex<T>> mPayload;
	};


	template<typename T>
	Mutex(T) -> Mutex<std::decay_t<T>>;

	template<typename T>
	SharedMutex(T) -> SharedMutex<std::decay_t<T>>;
} // namespace Strawberry::Core

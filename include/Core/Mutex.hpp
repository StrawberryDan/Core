#pragma once



#include <mutex>
#include "Core/Assert.hpp"
#include <memory>
#include "Core/Option.hpp"



namespace Strawberry::Core
{
	template<typename T>
	class Mutex;



	template <typename T>
	class MutexGuard
	{
		friend class Mutex<T>;

	public:
	    using LockType    = std::unique_lock<std::mutex>;

	    MutexGuard(const MutexGuard& other) = delete;
	    MutexGuard(MutexGuard&& other) = default;
	    MutexGuard& operator=(const MutexGuard&) = delete;
	    MutexGuard& operator=(MutexGuard&& other)  noexcept = default;

	    inline T& operator *() const { return *mPayload; }
	    inline T* operator->() const { return  mPayload; }

	private:
	    MutexGuard(std::mutex& mutex, T* ptr) : mLock(mutex), mPayload(ptr) {}

	    LockType     mLock;
	    T*           mPayload;
	};



	template<typename T>
	class Mutex
	{
	public:
	    template<typename ... Ts>
	    Mutex(Ts ... ts)
	        : mMutex()
	        , mPayload(std::forward<Ts>(ts)...)
	    {}

	    Mutex(Mutex&& rhs) requires ( std::is_move_constructible_v<T> )
	        : mMutex()
	        , mPayload(std::move(rhs).Take())
	    {}

	    Mutex& operator=(Mutex&& other) requires ( std::is_move_assignable_v<T> )
	    {
	        if (this != &other)
			{
				std::destroy_at(this);
				*Lock() = other.Take();
			}

			return *this;
	    }

	    ~Mutex()
	    {
	        auto lock = Lock();
	        std::destroy_at(&*lock);
	    }


	    MutexGuard<T>       Lock()       { return { mMutex, &mPayload}; }
	    MutexGuard<const T> Lock() const { return { mMutex, &mPayload}; }


	    Option<MutexGuard<T>> TryLock() &
	    {
	        std::unique_lock<std::mutex> lk(mMutex, std::defer_lock);
	        if (lk.try_lock())
	        {
	            return {lk, mPayload};
	        }
	        else
	        {
	            return {};
	        }
	    }

	    Option<MutexGuard<const T>> TryLock() const &
	    {
	        std::unique_lock<std::mutex> lk(mMutex, std::defer_lock);
	        if (lk.try_lock())
	        {
	            return {lk, &mPayload};
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
	    std::mutex mMutex;
	    union
		{
			T mPayload;
		};
	};



	template<typename T>
	class SharedMutex
	{
	public:
	    template<typename ... Ts>
	    SharedMutex(Ts ... ts) : mPayload(std::make_shared<Mutex<T>>(std::forward<Ts>(ts)...)) {}

	    MutexGuard<T>       Lock()       { return mPayload->Lock(); }
	    MutexGuard<const T> Lock() const { return mPayload->Lock(); }

	    Option<MutexGuard<T>>       TryLock()       { return mPayload->TryLock(); }
	    Option<MutexGuard<const T>> TryLock() const { return mPayload->TryLock(); }



	private:
	    std::shared_ptr<Mutex<T>> mPayload;
	};
}
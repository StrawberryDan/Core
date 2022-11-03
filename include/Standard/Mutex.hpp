#pragma once



#include <mutex>
#include "Standard/Assert.hpp"
#include <memory>
#include "Standard/Option.hpp"



namespace Strawberry::Standard
{
	template<typename T>
	class Mutex;



	template <typename T, bool IsConst>
	class MutexGuard
	{
	public:
	    using LockType    = std::unique_lock<std::mutex>;
	    using PayloadType = std::conditional_t<IsConst, const T, T>;

	    MutexGuard(const MutexGuard& other) = delete;
	    MutexGuard(MutexGuard&& other) = default;
	    MutexGuard& operator=(const MutexGuard&) = delete;
	    MutexGuard& operator=(MutexGuard&& other)  noexcept = default;

	    inline PayloadType& operator *() const { return *mPayload; }
	    inline PayloadType* operator->() const { return  mPayload; }
	private:
	    MutexGuard(LockType lock, PayloadType* ptr) : mLock(std::move(lock)), mPayload(ptr) {}

	    LockType     mLock;
	    PayloadType* mPayload;

	    friend class Mutex<T>;
	};



	template<typename T>
	class Mutex
	{
	public:
	    template<typename ... Ts>
	    Mutex(Ts ... ts)
	        : mMutex()
	        , mPayload{}
	    {
	        new (mPayload) T(std::forward<Ts>(ts)...);
	    }

	    Mutex(const Mutex& rhs) requires ( std::is_copy_constructible_v<T> )
	        : mMutex()
	        , mPayload{}
	    {
	        auto lock = rhs.Lock();
	        new (mPayload) T(*lock);
	    }

	    Mutex(Mutex&& rhs) requires ( std::is_move_constructible_v<T> )
	        : mMutex()
	        , mPayload{}
	    {
	        auto lock = rhs.Lock();
	        new (mPayload) T(std::forward<T>(*lock));
	    }

	    Mutex& operator=(const Mutex& other) requires ( std::is_copy_assignable_v<T> )
	    {
	        auto lockA = this->Lock();
	        auto lockB = other.Lock();
	        *lockA = *lockB;
	        return (*this);
	    }

	    Mutex& operator=(Mutex&& other) requires ( std::is_move_assignable_v<T> )
	    {
	        auto lockA = this->Lock();
	        auto lockB = other.Lock();
	        *lockA = std::move(*lockB);
	        return (*this);
	    }

	    ~Mutex()
	    {
	        auto lock = Lock();
	        lock->~T();
	    }


	    MutexGuard<T, false> Lock()       { return {typename MutexGuard<T, false>::LockType(mMutex), reinterpret_cast<      T*>(mPayload)}; }
	    MutexGuard<T,  true> Lock() const { return {typename MutexGuard<T,  true>::LockType(mMutex), reinterpret_cast<const T*>(mPayload)}; }


	    Option<MutexGuard<T, false>> TryLock()
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

	    Option<MutexGuard<T, false>> TryLock() const
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

	private:
	    std::mutex mMutex;
	    uint8_t    mPayload[sizeof(T)];
	};



	template<typename T>
	class SharedMutex
	{
	public:
	    template<typename ... Ts>
	    SharedMutex(Ts ... ts) : mPayload(std::make_shared<Mutex<T>>(std::forward<Ts>(ts)...)) {}

	    MutexGuard<T, false> Lock()       { return mPayload->Lock(); }
	    MutexGuard<T,  true> Lock() const { return mPayload->Lock(); }

	    Option<MutexGuard<T, false>> TryLock()       { return mPayload->TryLock(); }
	    Option<MutexGuard<T,  true>> TryLock() const { return mPayload->TryLock(); }



	private:
	    std::shared_ptr<Mutex<T>> mPayload;
	};
}
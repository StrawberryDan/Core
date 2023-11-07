#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <atomic>
#include <concepts>
#include <memory>
#include <utility>


namespace Strawberry::Core
{
	template<typename T>
	class EnableReflexivePointer;


	/// Checked pointer which knows when it's pointed to object is deleted or moved.
	template<typename T>
	class ReflexivePointer
	{
		template<typename>
		friend class EnableReflexivePointer;

		template<typename>
		friend class ReflexivePointer;


	public:
		ReflexivePointer()
			: mPtr(nullptr)
		{}


		ReflexivePointer(std::nullptr_t)
			: ReflexivePointer()
		{}


		explicit ReflexivePointer(const EnableReflexivePointer<T>& base) noexcept
				: ReflexivePointer(base.GetReflexivePointer())
		{}


		ReflexivePointer(const ReflexivePointer& rhs) noexcept
				: mPtr(rhs.mPtr)
		{}


		ReflexivePointer& operator=(const ReflexivePointer& rhs) noexcept
		{
			if (this != &rhs)
			{
				mPtr = rhs.mPtr;
			}

			return *this;
		}


		ReflexivePointer(ReflexivePointer&& rhs) noexcept
				: mPtr(std::move(rhs.mPtr))
		{}


		ReflexivePointer& operator=(ReflexivePointer&& rhs) noexcept
		{
			if (this != &rhs)
			{
				std::destroy_at(this);
				std::construct_at(this, std::move(rhs));
			}
			return *this;
		}


		template<typename T2> requires std::derived_from<T, T2>
		T2* Cast() const
		{
			return static_cast<T2*>(reinterpret_cast<T*>(mPtr->load()));
		}


		template<std::derived_from<T> T2>
		T2* Cast() const
		{
			return dynamic_cast<T2*>(reinterpret_cast<T*>(mPtr->load()));
		}


		bool operator==(const ReflexivePointer& rhs) const noexcept
		{
			return mPtr == rhs.mPtr;
		}


		bool operator!=(const ReflexivePointer& rhs) const noexcept
		{
			return mPtr != rhs.mPtr;
		}


		bool operator>(const ReflexivePointer& rhs) const noexcept
		{
			return Get() > rhs.Get();
		}


		bool operator>=(const ReflexivePointer& rhs) const noexcept
		{
			return Get() >= rhs.Get();
		}


		bool operator<(const ReflexivePointer& rhs) const noexcept
		{
			return Get() < rhs.Get();
		}


		bool operator<=(const ReflexivePointer& rhs) const noexcept
		{
			return Get() <= rhs.Get();
		}


		bool operator==(std::nullptr_t) const noexcept
		{
			return mPtr == nullptr;
		}


		bool operator!=(std::nullptr_t) const noexcept
		{
			return mPtr != nullptr;
		}


		T& operator*() const noexcept
		{
			Core::Assert(IsValid());
			return *reinterpret_cast<T*>(mPtr->load());
		}


		T* operator->() const noexcept
		{
			Core::Assert(IsValid());
			return reinterpret_cast<T*>(mPtr->load());
		}


		[[nodiscard]] bool IsValid() const noexcept
		{ return mPtr && *mPtr != nullptr; }


		explicit operator bool() const noexcept
		{ return IsValid(); }


		T* Get() const noexcept
		{ return IsValid() ? reinterpret_cast<T*>(mPtr.get()) : nullptr; }


		operator T*() const
		{ return Get(); }


	protected:
		explicit ReflexivePointer(std::shared_ptr<std::atomic<void*>> rawPtr) noexcept
				: mPtr(std::move(rawPtr))
		{
			static_assert(std::derived_from<T, EnableReflexivePointer<T>>);
		}


	private:
		std::shared_ptr<std::atomic<void*>> mPtr;
	};


	/// Base class which allows for ReflexivePointers to be made to it.
	template<typename T>
	class EnableReflexivePointer
	{
	public:
		EnableReflexivePointer() noexcept
				: mPtr(std::make_shared<std::atomic<void*>>(static_cast<void*>(this)))
		{
			static_assert(std::derived_from<T, EnableReflexivePointer<T>>);
		}


		EnableReflexivePointer(const EnableReflexivePointer& rhs) noexcept
				: EnableReflexivePointer()
		{
			// Do Nothing
		}


		EnableReflexivePointer& operator=(const EnableReflexivePointer& rhs) noexcept
		{
			// Do Nothing
			return *this;
		}


		EnableReflexivePointer(EnableReflexivePointer&& rhs) noexcept
				: mPtr(std::move(rhs.mPtr))
		{
			*mPtr = static_cast<T*>(this);
		}


		EnableReflexivePointer& operator=(EnableReflexivePointer&& rhs) noexcept
		{
			if (this != &rhs)
			{
				mPtr = std::move(rhs.mPtr);
				*mPtr = static_cast<T*>(this);
			}

			return *this;
		}


		~EnableReflexivePointer() = default;


		ReflexivePointer<T> GetReflexivePointer() const
		{
			return ReflexivePointer<T>(mPtr);
		}


	private:
		std::shared_ptr<std::atomic<void*>> mPtr;
	};
}

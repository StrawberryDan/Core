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
	class EnableReflexivePointer;

	template<typename T>
	class ReflexivePointer;


	/// Base class which allows for ReflexivePointers to be made to it.
	class EnableReflexivePointer
	{
	public:
		EnableReflexivePointer() noexcept
			: mPtr(std::make_shared<std::atomic<EnableReflexivePointer*>>(this)) {}


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
			ZoneScoped;

			*mPtr = this;
		}


		EnableReflexivePointer& operator=(EnableReflexivePointer&& rhs) noexcept
		{
			ZoneScoped;

			if (this != &rhs)
			{
				mPtr  = std::move(rhs.mPtr);
				*mPtr = this;
			}

			return *this;
		}


		virtual ~EnableReflexivePointer() = default;


		template<typename T>
		auto GetReflexivePointer(this T&& self) -> ReflexivePointer<std::decay_t<T>>
		{
			ZoneScoped;

			return ReflexivePointer<std::decay_t<T>>(self.mPtr);
		}


		template<typename T>
		auto GetConstReflexivePointer(this T&& self) -> ReflexivePointer<const std::decay_t<T>>
		{
			ZoneScoped;

			return ReflexivePointer<const std::decay_t<T>>(self.mPtr);
		}


	private:
		std::shared_ptr<std::atomic<EnableReflexivePointer*>> mPtr;
	};


	/// Checked pointer which knows when it's pointed to object is deleted or moved.
	template<typename T>
	class ReflexivePointer
	{
		template <typename>
		friend class ReflexivePointer;
		friend class EnableReflexivePointer;


	public:
		ReflexivePointer()
			: mPtr(nullptr) {}


		ReflexivePointer(std::nullptr_t)
			: ReflexivePointer() {}


		template <typename Base> requires (std::derived_from<std::decay_t<Base>, EnableReflexivePointer>)
		explicit ReflexivePointer(Base&& base) noexcept
			: ReflexivePointer(std::forward<Base>(base).GetReflexivePointer()) {}


		ReflexivePointer(const ReflexivePointer& rhs) noexcept
			: mPtr(rhs.mPtr) {}


		ReflexivePointer& operator=(const ReflexivePointer& rhs) noexcept
		{
			ZoneScoped;

			if (this != &rhs)
			{
				mPtr = rhs.mPtr;
			}

			return *this;
		}


		ReflexivePointer(ReflexivePointer&& rhs) noexcept
			: mPtr(std::move(rhs.mPtr)) {}


		ReflexivePointer& operator=(ReflexivePointer&& rhs) noexcept
		{
			ZoneScoped;

			if (this != &rhs)
			{
				std::destroy_at(this);
				std::construct_at(this, std::move(rhs));
			}
			return *this;
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
			ZoneScoped;

			Assert(IsValid());
			return *static_cast<T*>(mPtr->load());
		}


		T* operator->() const noexcept
		{
			ZoneScoped;

			Assert(IsValid());
			return static_cast<T*>(mPtr->load());
		}


		[[nodiscard]] bool IsValid() const noexcept
		{
			ZoneScoped;

			return mPtr && *mPtr != nullptr;
		}


		explicit operator bool() const noexcept
		{
			ZoneScoped;

			return IsValid();
		}


		T* Get() const noexcept
		{
			ZoneScoped;

			return IsValid() ? static_cast<T*>(mPtr->load()) : nullptr;
		}


		operator T*() const
		{
			return Get();
		}


		operator ReflexivePointer<const T>() const
		{
			return ReflexivePointer<const T>(mPtr);
		}


	protected:
		explicit ReflexivePointer(std::shared_ptr<std::atomic<EnableReflexivePointer*>> rawPtr) noexcept
			: mPtr(std::move(rawPtr)) {}

	private:
		std::shared_ptr<std::atomic<EnableReflexivePointer*>> mPtr;;
	};
}

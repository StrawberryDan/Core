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
                *mPtr = this;
            }


            EnableReflexivePointer& operator=(EnableReflexivePointer&& rhs) noexcept
            {
                if (this != &rhs)
                {
                    mPtr  = std::move(rhs.mPtr);
                    *mPtr = this;
                }

                return *this;
            }


            virtual ~EnableReflexivePointer() = default;


            template<typename T>
            auto GetReflexivePointer(this const T& self) -> ReflexivePointer<T>
            {
                return ReflexivePointer<T>(self.mPtr);
            }

        private:
            std::shared_ptr<std::atomic<EnableReflexivePointer*>> mPtr;
    };


    /// Checked pointer which knows when it's pointed to object is deleted or moved.
    template<typename T>
    class ReflexivePointer
    {
        friend class EnableReflexivePointer;

        public:
            ReflexivePointer()
                : mPtr(nullptr) {}


            ReflexivePointer(std::nullptr_t)
                : ReflexivePointer() {}


            explicit ReflexivePointer(EnableReflexivePointer& base) noexcept
                : ReflexivePointer(base.GetReflexivePointer()) {}


            explicit ReflexivePointer(const EnableReflexivePointer& base) noexcept
                : ReflexivePointer(base.GetReflexivePointer()) {}


            ReflexivePointer(const ReflexivePointer& rhs) noexcept
                : mPtr(rhs.mPtr) {}


            ReflexivePointer& operator=(const ReflexivePointer& rhs) noexcept
            {
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
                Core::Assert(IsValid());
                return static_cast<T>(*mPtr);
            }


            T* operator->() const noexcept
            {
                Core::Assert(IsValid());
                return static_cast<T*>(mPtr->load());
            }


            [[nodiscard]] bool IsValid() const noexcept
            {
                return mPtr && *mPtr != nullptr;
            }


            explicit operator bool() const noexcept
            {
                return IsValid();
            }


            T* Get() const noexcept
            {
                return IsValid() ? static_cast<T*>(mPtr->load()) : nullptr;
            }


            operator T*() const
            {
                return Get();
            }

        protected:
            explicit ReflexivePointer(std::shared_ptr<std::atomic<EnableReflexivePointer*>> rawPtr) noexcept
                : mPtr(std::move(rawPtr)) {}

        private:
            std::shared_ptr<std::atomic<EnableReflexivePointer*>> mPtr;;
    };
}

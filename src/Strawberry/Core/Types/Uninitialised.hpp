#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Strawberry/Core/Assert.hpp"
// Standard Library
#include <concepts>
#include <memory>
#include <cstring>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template<typename T>
	class Uninitialised
	{
		public:
			Uninitialised()
#if STRAWBERRY_DEBUG
				: mInitialised(false)
#endif // STRAWBERRY_DEBUG
			{}


			Uninitialised(const Uninitialised& rhs)
#if STRAWBERRY_DEBUG
				: mInitialised(rhs.mInitialised)
#endif // STRAWBERRY_DEBUG
			{
				std::memcpy(&mPayload, &rhs.mPayload, sizeof(T));
			}


			Uninitialised& operator=(const Uninitialised& rhs)
			{
#if STRAWBERRY_DEBUG
				mInitialised = rhs.mInitialised;
#endif // STRAWBERRY_DEBUG
				std::memcpy(&mPayload, &rhs.mPayload, sizeof(T));
			}


			Uninitialised(Uninitialised&&) = delete;


			Uninitialised operator=(Uninitialised&&) = delete;


			~Uninitialised()
			{
#if STRAWBERRY_DEBUG
				Core::Assert(!mInitialised);
#endif // STRAWBERRY_DEBUG
			}


			template<typename... Args> requires std::constructible_from<T, Args...>
			void Construct(Args... args)
			{
				std::construct_at(&mPayload, std::forward<Args>(args)...);
#if STRAWBERRY_DEBUG
				Core::Assert(!mInitialised);
				mInitialised = true;
#endif // STRAWBERRY_DEBUG
			}


			void Destruct()
			{
#if STRAWBERRY_DEBUG
				Core::Assert(mInitialised);
				mInitialised = false;
#endif // STRAWBERRY_DEBUG
				std::destroy_at(&mPayload);
			}


			T& Get()
			{
				return mPayload;
			}


			const T& Get() const
			{
				return mPayload;
			}


			T& operator*()
			{
				return mPayload;
			}


			const T& operator*() const
			{
				return mPayload;
			}


			T* operator->()
			{
				return &mPayload;
			}


			const T* operator->() const
			{
				return &mPayload;
			}

		private:
#if STRAWBERRY_DEBUG
			bool mInitialised = false;
#endif // STRAWBERRY_DEBUG
			union
			{
				T mPayload;
			};
	};
} // namespace Strawberry::Core

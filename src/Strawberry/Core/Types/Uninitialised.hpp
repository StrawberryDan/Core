#pragma once

//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include <concepts>
#include <memory>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template <typename T>
	class Uninitialised
	{
	public:
		Uninitialised() {}


		Uninitialised(const Uninitialised& rhs)
#if !NDEBUG
			: mInitialised(rhs.mInitialised)
#endif // !NDEBUG
		{
			std::memcpy(&mPayload, &rhs.mPayload, sizeof(T));
		}


		Uninitialised& operator=(const Uninitialised& rhs)
		{
#if !NDEBUG
			mInitialised = rhs.mInitialised;
#endif // !NDEBUG
			std::memcpy(&mPayload, &rhs.mPayload, sizeof(T));
		}


		Uninitialised(Uninitialised&&)           = delete;


		Uninitialised operator=(Uninitialised&&) = delete;


		~Uninitialised()
		{
#if !NDEBUG
			Core::Assert(!mInitialised);
#endif // !NDEBUG
		}

		template <typename... Args>
			requires std::constructible_from<T, Args...>
		void Construct(Args... args)
		{
			std::construct_at(&mPayload, std::forward<Args>(args)...);
#if !NDEBUG
			Core::Assert(!mInitialised);
			mInitialised = true;
#endif // !NDEBUG
		}

		void Destruct()
		{
#if !NDEBUG
			static_assert(false);
			Core::Assert(mInitialised);
			mInitialised = false;
#endif // !NDEBUG
			std::destroy_at(&mPayload);
		}

		T& Get() { return mPayload; }

		const T& Get() const { return mPayload; }


		T& operator*() { return mPayload; }

		const T& operator*() const { return mPayload; }


		T* operator->() { return &mPayload; }

		const T* operator->() const { return &mPayload; }

	private:
#if !NDEBUG
		bool mInitialised = false;
#endif // !NDEBUG
		union
		{
			T mPayload;
		};
	};
} // namespace Strawberry::Core

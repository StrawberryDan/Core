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

		Uninitialised(const Uninitialised& rhs) { std::memcpy(&mPayload, &rhs.mPayload, sizeof(T)); }

		Uninitialised& operator=(const Uninitialised& rhs) { std::memcpy(&mPayload, &rhs.mPayload, sizeof(T)); }

		Uninitialised(Uninitialised&&)           = delete;

		Uninitialised operator=(Uninitialised&&) = delete;

		~Uninitialised() {}

		template <typename... Args>
			requires std::constructible_from<T, Args...>
		void Construct(Args... args)
		{
			std::construct_at(&mPayload, std::forward<Args>(args)...);
		}

		void Destruct() { std::destroy_at(&mPayload); }

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

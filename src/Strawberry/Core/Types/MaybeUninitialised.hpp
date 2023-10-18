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
	class MaybeUninitialised
	{
	public:
		MaybeUninitialised() {}

		MaybeUninitialised(const MaybeUninitialised& rhs) { std::memcpy(&mPayload, &rhs.mPayload, sizeof(T)); }

		MaybeUninitialised& operator=(const MaybeUninitialised& rhs) { std::memcpy(&mPayload, &rhs.mPayload, sizeof(T)); }

		MaybeUninitialised(MaybeUninitialised&&)           = delete;

		MaybeUninitialised operator=(MaybeUninitialised&&) = delete;

		~MaybeUninitialised() {}

		template <typename... Args>
			requires std::constructible_from<T, Args...>
		void Construct(Args... args)
		{
			std::construct_at(&mPayload, std::forward<Args...>(args)...);
		}

		void Destruct() { std::destroy_at(&mPayload); }

		T& Get() { return mPayload; }

		const T& Get() const { return mPayload; }

		T& operator*() { return mPayload; }

		const T& operator*() const { return mPayload; }

		T* operator->() { return &mPayload; }

		const T* operator->() const { return &mPayload; }

	private:
		union
		{
			T mPayload;
		};
	};
} // namespace Strawberry::Core
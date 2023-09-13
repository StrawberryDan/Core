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

		MaybeUninitialised(const MaybeUninitialised&)            = delete;
		MaybeUninitialised(MaybeUninitialised&&)                 = delete;
		MaybeUninitialised& operator=(const MaybeUninitialised&) = delete;
		MaybeUninitialised& operator=(MaybeUninitialised&&)      = delete;

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

	private:
		union
		{
			T mPayload;
		};
	};
} // namespace Strawberry::Core
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
	template <std::copyable T>
	class COW
	{
	public:
		COW()
			requires std::default_initializable<T>
			: mPayload(std::make_shared<T>())
		{}

		COW(T value)
			requires std::move_constructible<T>
			: mPayload(std::make_shared<T>(std::move(value)))
		{}

		template <typename... Args>
		COW(Args... args)
			requires std::constructible_from<T, Args...>
			: mPayload(std::make_shared<T>(std::forward<Args>(args)...))
		{}

		const T& Get() { return *mPayload; }

		T& GetMutable()
		{
			Clone();
			return *mPayload;
		}

		const T& operator*() const
		{
			mPayload = std::make_shared<T>(*mPayload);
			return *mPayload;
		}

		const T* operator->() const { return mPayload.get(); }

	protected:
		void Clone() { mPayload = std::make_shared<T>(*mPayload); }

	private:
		std::shared_ptr<T> mPayload;
	};
} // namespace Strawberry::Core

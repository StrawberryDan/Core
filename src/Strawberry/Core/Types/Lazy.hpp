#pragma once
//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
// Standard Library
#include <concepts>
#include <functional>

//======================================================================================================================
//  Class Declaration
//----------------------------------------------------------------------------------------------------------------------
namespace Strawberry::Core
{
	template<typename T>
	class Lazy
	{
		public:
			T& Get()
			{
				if (!mInitialised)
				{
					T payload = mInitialiser();
					std::destroy_at(&mInitialiser);
					std::construct_at(&mPayload, std::move(payload));
					mInitialised = true;
				}

				return mPayload;
			}


			const T& Get() const
			{
				if (!mInitialised)
				{
					T payload = mInitialiser();
					std::destroy_at(mInitialiser);
					std::construct_at(mPayload, std::move(payload));
					mInitialised = true;
				}

				return mPayload;
			}

		public:
			Lazy() requires (std::default_initializable<T>)
				: mInitialiser([]() -> T
				{
					return T();
				}) {}


			template<typename F>
			explicit Lazy(F f)
				: mInitialiser(std::move(f)) {}


			Lazy(const Lazy& rhs)
				: mInitialised(std::copyable<T> && rhs.mInitialised)
			{
				if (std::copyable<T> && mInitialised)
				{
					std::construct_at(&mPayload, rhs.mPayload);
				}
				else
				{
					std::construct_at(&mInitialiser, rhs.mInitialiser);
				}
			}


			~Lazy()
			{
				if (!mInitialised)
				{
					std::destroy_at(&mInitialiser);
				}
				else
				{
					std::destroy_at(&mPayload);
				}
			}

		protected:
			mutable bool mInitialised = false;


			union
			{
				T                  mPayload;
				std::function<T()> mInitialiser;
			};
	};
} // namespace Strawberry::Core

#pragma once


//======================================================================================================================
//  Includes
//----------------------------------------------------------------------------------------------------------------------
#include "Option.hpp"
#include "Concepts.hpp"


namespace Strawberry::Core
{
	template<typename T>
	class DynamicEvaluation
	{
	public:
		DynamicEvaluation(Callable<T> auto functor)
			: mDirty(true)
			, mFunctor(std::move(functor))
			, mValue()
		{}


		void Recalculate()
		{
			mValue = std::invoke(mFunctor);
			Assert(mValue.HasValue());
			mDirty = false;
		}

		void MakeDirty()
		{
			mDirty = true;
		}


		      T& Read()       { if (mDirty) Recalculate(); return *mValue; }
		const T& Read() const { if (mDirty) Recalculate(); return *mValue; }

		      T& operator*()       { return Read(); }
		const T& operator*() const { return Read(); }

		      T* operator->()       { return &Read(); }
		const T* operator->() const { return &Read(); }


	private:
		bool               mDirty;
		std::function<T()> mFunctor;
		Option<T>          mValue;
	};
}

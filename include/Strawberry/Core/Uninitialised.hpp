#pragma once



#include <utility>



namespace Strawberry::Core
{
	template <typename T>
	class Uninitialised
	{
	public:
		Uninitialised() {}
		Uninitialised(const T& value)
			: mValue(value)
		{}

		Uninitialised(T&& value)
			: mValue(std::move(value))
		{}

		Uninitialised(const Uninitialised&)             = delete;
		Uninitialised(Uninitialised&&)                  = delete;
		Uninitialised& operator= (const Uninitialised&) = delete;
		Uninitialised& operator= (Uninitialised&&)      = delete;

		~Uninitialised() {}

		T& Value() { return mValue; }
		T& operator*() { return mValue; }
		explicit operator T() const { return Value(); }
	private:
		union
		{
			T mValue;
		};
	};
}

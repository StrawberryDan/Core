#pragma once



#include <utility>



namespace Strawberry::Core
{
	template <typename T>
	class Uninitialised
	{
	public:
		Uninitialised() = default;
		Uninitialised(T value)
			: value(std::move(value))
		{}

		operator T() const { return value;}
	private:
		union
		{
			T value;
		};
	};
}

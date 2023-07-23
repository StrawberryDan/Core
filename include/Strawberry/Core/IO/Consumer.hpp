#pragma once



namespace Strawberry::Core::IO
{
	template<typename T>
	class Consumer
	{
	public:
		bool Feed(T&&) = 0;
	};
}
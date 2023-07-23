#pragma once



namespace Strawberry::Core::IO
{
	template<typename T>
	class Consumer
	{
	public:
		virtual bool Feed(T&&) = 0;
	};
}
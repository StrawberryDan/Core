#pragma once



namespace Strawberry::Core::Net
{
	class SocketAPI
	{
	public:
		static void Initialise();
		~SocketAPI();


	private:
		SocketAPI();
	};
}
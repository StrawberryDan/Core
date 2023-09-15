#include "Strawberry/Core/Net/Socket/API.hpp"


#include "Strawberry/Core/Util/Logging.hpp"


#if defined(_WIN32)


#include <winsock2.h>
#include "Strawberry/Core/Util/Assert.hpp"


#endif


namespace Strawberry::Core::Net::Socket
{
	bool API::sIsInitialised = false;


	void API::Initialise()
	{
		if (!IsInitialised())
		{
			Logging::Info("Initialising Socket API");
#if defined(_WIN32)
			WSAData wsaData;
			auto err = WSAStartup(MAKEWORD(2, 2), &wsaData);
			Assert(err == 0);
#endif
			sIsInitialised = true;
		}
	}


	void API::Terminate()
	{
		if (IsInitialised())
		{
			Logging::Info("Terminating Socket API");
#if defined(_WIN32)
			WSACleanup();
#endif
			sIsInitialised = false;
		}
	}


	bool API::IsInitialised()
	{
		return sIsInitialised;
	}
} // namespace Strawberry::Core::Net::Socket
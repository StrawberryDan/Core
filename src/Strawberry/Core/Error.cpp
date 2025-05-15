#include "Error.hpp"


#ifdef STRAWBERRY_TARGET_WINDOWS
#include <windows.h>
#endif



namespace Strawberry::Core
{
	ErrorSystem::ErrorSystem()
	{
#ifdef STRAWBERRY_TARGET_WINDOWS
		mErrorCode = GetLastError();
#endif
	}
}
